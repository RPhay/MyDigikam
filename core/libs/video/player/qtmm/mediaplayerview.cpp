/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-20-12
 * Description : a view to embed QtMultimedia media player.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "mediaplayerview.h"

// Qt includes

#include <QApplication>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QProxyStyle>
#include <QPushButton>
#include <QToolButton>
#include <QFileInfo>
#include <QToolBar>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QFrame>
#include <QStyle>
#include <QMenu>
#include <QTransform>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QVideoSink>
#include <QVideoFrame>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QStandardPaths>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "thememanager.h"
#include "stackedview.h"
#include "dlayoutbox.h"
#include "metaengine.h"
#include "dmetadata.h"

namespace Digikam
{

class Q_DECL_HIDDEN MediaPlayerMouseClickFilter : public QObject
{
    Q_OBJECT

public:

    explicit MediaPlayerMouseClickFilter(QObject* const parent)
        : QObject (parent),
          m_parent(parent)
    {
    }

protected:

    bool eventFilter(QObject* obj, QEvent* event) override
    {
        if ((event->type() == QEvent::MouseButtonPress) || (event->type() == QEvent::MouseButtonDblClick))
        {
            bool singleClick              = qApp->style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick);
            QMouseEvent* const mouseEvent = dynamic_cast<QMouseEvent*>(event);

            if (m_parent && mouseEvent)
            {
                MediaPlayerView* const mplayer = dynamic_cast<MediaPlayerView*>(m_parent);

                if (mplayer)
                {
                    if      (
                             (mouseEvent->button() == Qt::LeftButton) &&
                             (
                              (singleClick  && (event->type() == QEvent::MouseButtonPress)) ||
                              (!singleClick && (event->type() == QEvent::MouseButtonDblClick))
                             )
                            )
                    {
                        mplayer->slotEscapePressed();
                    }
                    else if (
                             (mouseEvent->button() == Qt::RightButton) &&
                             (event->type()        == QEvent::MouseButtonPress)
                            )
                    {
                        mplayer->slotRotateVideo();
                    }

                    return true;
                }
            }
        }

        return QObject::eventFilter(obj, event);
    }

private:

    QObject* m_parent = nullptr;
};

// --------------------------------------------------------

class Q_DECL_HIDDEN PlayerVideoStyle : public QProxyStyle
{
    Q_OBJECT

public:

    using QProxyStyle::QProxyStyle;

    int styleHint(QStyle::StyleHint hint,
                  const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override
    {
        if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
        {
            return (Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);
        }

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

// --------------------------------------------------------

class MediaPlayerView::Private
{

public:

    enum MediaPlayerViewMode
    {
        ErrorView = 0,
        PlayerView
    };

public:

    Private() = default;

    QFrame*              errorView          = nullptr;
    QFrame*              playerView         = nullptr;

    QAction*             prevAction         = nullptr;
    QAction*             nextAction         = nullptr;
    QAction*             playAction         = nullptr;
    QAction*             grabAction         = nullptr;
    QAction*             backAction         = nullptr;
    QAction*             forwAction         = nullptr;

    QToolButton*         rateButton         = nullptr;

    QPushButton*         loopPlay           = nullptr;

    QToolBar*            toolBar            = nullptr;

    DInfoInterface*      iface              = nullptr;

    QGraphicsScene*      videoScene         = nullptr;
    QGraphicsView*       videoView          = nullptr;
    QGraphicsVideoItem*  videoItem          = nullptr;
    QMediaPlayer*        player             = nullptr;
    QAudioOutput*        audio              = nullptr;

    QSlider*             slider             = nullptr;
    QSlider*             volume             = nullptr;
    QLabel*              tlabel             = nullptr;
    QLabel*              speaker            = nullptr;
    QUrl                 currentItem;

    int                  videoOrientation   = 0;
    qint64               sliderTime         = 0;

    const QUrl           dummyVideo         = QUrl::fromLocalFile(
                                                  QStandardPaths::locate(
                                                      QStandardPaths::GenericDataLocation,
                                                      QLatin1String("digikam/data/video-digikam.mp4")));

public:

    void adjustVideoSize()
    {
        videoItem->resetTransform();

        QSizeF nativeSize    = videoItem->nativeSize();
        int mediaOrientation = videoMediaOrientation();

        if (
            (nativeSize.width()  < 1.0) ||
            (nativeSize.height() < 1.0)
           )
        {
            return;
        }

        if (
            (mediaOrientation == 90) ||
            (mediaOrientation == 270)
           )
        {
            nativeSize.transpose();
        }

        double ratio = (nativeSize.width() /
                        nativeSize.height());

        if (videoView->width() > videoView->height())
        {
            QSizeF vsize(videoView->height() * ratio,
                         videoView->height());
            videoItem->setSize(vsize);
        }
        else
        {
            QSizeF vsize(videoView->width(),
                         videoView->width() / ratio);
            videoItem->setSize(vsize);
        }

        videoView->setSceneRect(0, 0, videoItem->size().width(),
                                      videoItem->size().height());

        QPointF center = videoItem->boundingRect().center();
        videoItem->setTransformOriginPoint(center);
        videoItem->setRotation(videoOrientation);

        videoView->fitInView(videoItem, Qt::KeepAspectRatio);
        videoView->centerOn(videoItem);
        videoView->raise();
    };

    int videoMediaOrientation() const
    {
        int orientation = 0;
        QVariant val    = player->metaData().value(QMediaMetaData::Orientation);

        if (!val.isNull())
        {
            orientation = val.toInt();
        }

        return orientation;
    };

    void setVideoItemOrientation(int orientation)
    {
        videoOrientation = orientation;
        adjustVideoSize();
    };
};

MediaPlayerView::MediaPlayerView(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    const int spacing      = layoutSpacing();


    d->prevAction          = new QAction(QIcon::fromTheme(QLatin1String("go-previous")),
                                         i18nc("go to previous image", "Back"),           this);
    d->nextAction          = new QAction(QIcon::fromTheme(QLatin1String("go-next")),
                                         i18nc("go to next image", "Forward"),            this);
    d->backAction          = new QAction(QIcon::fromTheme(QLatin1String("media-seek-backward")),
                                         i18nc("video frame backward", "Frame Backward"), this);
    d->playAction          = new QAction(QIcon::fromTheme(QLatin1String("media-playback-start")),
                                         i18nc("pause/play video", "Pause/Play"),         this);
    d->forwAction          = new QAction(QIcon::fromTheme(QLatin1String("media-seek-forward")),
                                         i18nc("video frame forward", "Frame Forward"),   this);
    d->grabAction          = new QAction(QIcon::fromTheme(QLatin1String("view-preview")),
                                         i18nc("capture video frame", "Capture"),         this);

    d->rateButton          = new QToolButton(this);
    d->rateButton->setIcon(QIcon::fromTheme(QLatin1String("filename-bpm-amarok")));
    d->rateButton->setPopupMode(QToolButton::InstantPopup);
    d->rateButton->setArrowType(Qt::NoArrow);

    QMenu* const rateMenu         = new QMenu(this);
    QActionGroup* const rateGroup = new QActionGroup(this);
    rateGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

    QAction* const rate05         = rateGroup->addAction(i18nc("video play speed", "0.5x"));
    rate05->setCheckable(true);
    rate05->setData(0.5);
    QAction* const rate10         = rateGroup->addAction(i18nc("video play speed", "1.0x"));
    rate10->setCheckable(true);
    rate10->setData(1.0);
    QAction* const rate15         = rateGroup->addAction(i18nc("video play speed", "1.5x"));
    rate15->setCheckable(true);
    rate15->setData(1.5);
    QAction* const rate20         = rateGroup->addAction(i18nc("video play speed", "2.0x"));
    rate20->setCheckable(true);
    rate20->setData(2.0);
    QAction* const rate25         = rateGroup->addAction(i18nc("video play speed", "2.5x"));
    rate25->setCheckable(true);
    rate25->setData(2.5);
    QAction* const rate30         = rateGroup->addAction(i18nc("video play speed", "3.0x"));
    rate30->setCheckable(true);
    rate30->setData(3.0);
    QAction* const rate40         = rateGroup->addAction(i18nc("video play speed", "4.0x"));
    rate40->setCheckable(true);
    rate40->setData(4.0);
    QAction* const rate50         = rateGroup->addAction(i18nc("video play speed", "5.0x"));
    rate50->setCheckable(true);
    rate50->setData(5.0);

    rate10->setChecked(true);

    rateMenu->addAction(rate05);
    rateMenu->addAction(rate10);
    rateMenu->addAction(rate15);
    rateMenu->addAction(rate20);
    rateMenu->addAction(rate25);
    rateMenu->addAction(rate30);
    rateMenu->addAction(rate40);
    rateMenu->addAction(rate50);
    d->rateButton->setMenu(rateMenu);

    d->errorView           = new QFrame(this);
    QLabel* const errorMsg = new QLabel(i18n("An error has occurred with the media player..."), this);

    errorMsg->setAlignment(Qt::AlignCenter);
    d->errorView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->errorView->setLineWidth(1);

    QVBoxLayout* const vbox1 = new QVBoxLayout(d->errorView);
    vbox1->addWidget(errorMsg, 10);
    vbox1->setContentsMargins(QMargins());
    vbox1->setSpacing(spacing);

    insertWidget(Private::ErrorView, d->errorView);

    // --------------------------------------------------------------------------

    d->playerView = new QFrame(this);
    d->videoScene = new QGraphicsScene(this);
    d->videoView  = new QGraphicsView(d->videoScene);
    d->videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->videoView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    d->videoView->setFrameShape(QFrame::NoFrame);
    d->videoItem  = new QGraphicsVideoItem();
    d->player     = new QMediaPlayer(this);
    d->audio      = new QAudioOutput;
    d->player->setAudioOutput(d->audio);
    d->player->setVideoOutput(d->videoItem);
    d->videoScene->addItem(d->videoItem);

    d->playerView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->playerView->setLineWidth(1);

    d->videoItem->setAspectRatioMode(Qt::IgnoreAspectRatio);
    d->videoView->setMouseTracking(true);

    DHBox* const hbox = new DHBox(this);
    d->slider         = new QSlider(Qt::Horizontal, hbox);
    d->slider->setStyle(new PlayerVideoStyle());
    d->slider->setRange(0, 0);
    d->tlabel         = new QLabel(hbox);
    d->tlabel->setText(QLatin1String("00:00:00 / 00:00:00"));
    d->loopPlay       = new QPushButton(hbox);
    d->loopPlay->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
    d->loopPlay->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    d->loopPlay->setToolTip(i18n("Toggle playing in a loop"));
    d->loopPlay->setFocusPolicy(Qt::NoFocus);
    d->loopPlay->setMinimumSize(22, 22);
    d->loopPlay->setCheckable(true);
    d->speaker        = new QLabel(hbox);
    d->speaker->setPixmap(QIcon::fromTheme(QLatin1String("audio-volume-high")).pixmap(22, 22));
    d->volume         = new QSlider(Qt::Horizontal, hbox);
    d->volume->setRange(0, 100);
    d->volume->setValue(50);

    hbox->setStretchFactor(d->slider, 10);
    hbox->setContentsMargins(0, 0, 0, spacing);
    hbox->setSpacing(spacing);

    QVBoxLayout* const vbox2 = new QVBoxLayout(d->playerView);
    vbox2->addWidget(d->videoView, 10);
    vbox2->addWidget(hbox,          0);
    vbox2->setContentsMargins(QMargins());
    vbox2->setSpacing(spacing);

    insertWidget(Private::PlayerView, d->playerView);

    d->toolBar               = new QToolBar(this);
    d->toolBar->addAction(d->prevAction);
    d->toolBar->addAction(d->nextAction);
    d->toolBar->addAction(d->backAction);
    d->toolBar->addAction(d->playAction);
    d->toolBar->addAction(d->forwAction);
    d->toolBar->addWidget(d->rateButton);
    d->toolBar->addAction(d->grabAction);
    d->toolBar->setStyleSheet(toolButtonStyleSheet());

    setPreviewMode(Private::PlayerView);

    d->errorView->installEventFilter(new MediaPlayerMouseClickFilter(this));
    d->videoView->installEventFilter(new MediaPlayerMouseClickFilter(this));
    d->playerView->installEventFilter(this);

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Media Player Settings"));
    int volume                = group.readEntry("Volume", 50);

    d->volume->setValue(volume);
    d->audio->setVolume(volume / 100.0F);

    // --------------------------------------------------------------------------

    connect(ThemeManager::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));

    connect(d->prevAction, SIGNAL(triggered()),
            this, SIGNAL(signalPrevItem()));

    connect(d->nextAction, SIGNAL(triggered()),
            this, SIGNAL(signalNextItem()));

    connect(d->backAction, SIGNAL(triggered()),
            this, SLOT(slotFrameBackward()));

    connect(d->playAction, SIGNAL(triggered()),
            this, SLOT(slotPausePlay()));

    connect(d->forwAction, SIGNAL(triggered()),
            this, SLOT(slotFrameForward()));

    connect(d->grabAction, SIGNAL(triggered()),
            this, SLOT(slotCapture()));

    connect(d->slider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotPosition(int)));

    connect(d->slider, SIGNAL(valueChanged(int)),
            this, SLOT(slotPosition(int)));

    connect(d->volume, SIGNAL(valueChanged(int)),
            this, SLOT(slotVolumeChanged(int)));

    connect(d->loopPlay, SIGNAL(toggled(bool)),
            this, SLOT(slotLoopToggled(bool)));

    connect(d->player, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)),
            this, SLOT(slotPlayerStateChanged(QMediaPlayer::PlaybackState)));

    connect(d->player, SIGNAL(positionChanged(qint64)),
            this, SLOT(slotPositionChanged(qint64)));

    connect(d->player, SIGNAL(durationChanged(qint64)),
            this, SLOT(slotDurationChanged(qint64)));

    connect(d->player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(slotMediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(d->player, SIGNAL(errorOccurred(QMediaPlayer::Error,QString)),
            this, SLOT(slotHandlePlayerError(QMediaPlayer::Error,QString)));

    connect(d->videoItem, SIGNAL(nativeSizeChanged(QSizeF)),
            this, SLOT(slotNativeSizeChanged()));

    connect(rateMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotPlaybackRate(QAction*)));
}

MediaPlayerView::~MediaPlayerView()
{
    escapePreview();

    delete d;
}

void MediaPlayerView::setInfoInterface(DInfoInterface* const iface)
{
    d->iface = iface;
}

void MediaPlayerView::reload()
{
    d->player->stop();
    d->player->setSource(d->currentItem);
    d->player->play();
}

void MediaPlayerView::slotPlayerStateChanged(QMediaPlayer::PlaybackState newState)
{
    if (newState == QMediaPlayer::PlayingState)
    {
        d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));

        d->backAction->setEnabled(true);
        d->forwAction->setEnabled(true);
    }
    else
    {
        d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));

        if (
            (newState                 == QMediaPlayer::StoppedState) ||
            (d->player->mediaStatus() == QMediaPlayer::EndOfMedia)
           )
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Play video with QtMultimedia completed:" << d->player->source();

            if (d->player->error() != QMediaPlayer::NoError)
            {
                setPreviewMode(Private::ErrorView);
            }

            d->backAction->setEnabled(false);
            d->forwAction->setEnabled(false);
        }
    }
}

void MediaPlayerView::slotMediaStatusChanged(QMediaPlayer::MediaStatus newStatus)
{
    if      (newStatus == QMediaPlayer::LoadedMedia)
    {
        int rotate = d->videoMediaOrientation();

        qCDebug(DIGIKAM_GENERAL_LOG) << "Found video orientation with QtMultimedia:"
                                     << rotate;

        rotate     = (-rotate) + d->videoOrientation;

        if ((rotate > 270) || (rotate < 0))
        {
            rotate = d->videoOrientation;
        }

        d->setVideoItemOrientation(rotate);
    }
    else if (newStatus == QMediaPlayer::InvalidMedia)
    {
        setPreviewMode(Private::ErrorView);
    }
}

void MediaPlayerView::escapePreview()
{
    d->player->stop();
    d->player->setSource(d->dummyVideo);
}

void MediaPlayerView::slotThemeChanged()
{
    QPalette palette;
    palette.setColor(d->errorView->backgroundRole(), qApp->palette().color(QPalette::Base));
    d->errorView->setPalette(palette);

    QPalette palette2;
    palette2.setColor(d->playerView->backgroundRole(), qApp->palette().color(QPalette::Base));
    d->playerView->setPalette(palette2);

    d->speaker->setPixmap(QIcon::fromTheme(QLatin1String("audio-volume-high")).pixmap(22, 22));
}

void MediaPlayerView::slotEscapePressed()
{
    Q_EMIT signalEscapePreview();
}

void MediaPlayerView::slotRotateVideo()
{
    if (d->player->isPlaying())
    {
        int orientation = 0;

        switch (d->videoOrientation)
        {
            case 0:
            {
                orientation = 90;
                break;
            }

            case 90:
            {
                orientation = 180;
                break;
            }

            case 180:
            {
                orientation = 270;
                break;
            }

            default:
            {
                orientation = 0;
                break;
            }
        }

        d->setVideoItemOrientation(orientation);
    }
}

void MediaPlayerView::slotPausePlay()
{
    if (!d->player->isPlaying())
    {
        d->player->play();

        return;
    }

    d->player->pause();
}

void MediaPlayerView::slotFrameBackward()
{
    if (d->player->isPlaying())
    {
        d->player->pause();

        return;
    }

    QVariant frameRateVar = d->player->metaData().value(QMediaMetaData::VideoFrameRate);

    if (frameRateVar.isValid())
    {
        double frameRate = frameRateVar.toDouble();

        if (frameRate > 0.0)
        {
           double frame = 1000.0 / frameRate;
           d->player->setPosition(d->player->position() - frame);
        }
    }
}

void MediaPlayerView::slotFrameForward()
{
    if (d->player->isPlaying())
    {
        d->player->pause();

        return;
    }

    QVariant frameRateVar = d->player->metaData().value(QMediaMetaData::VideoFrameRate);

    if (frameRateVar.isValid())
    {
        double frameRate = frameRateVar.toDouble();

        if (frameRate > 0.0)
        {
           double frame = 1000.0 / frameRate;
           d->player->setPosition(d->player->position() + frame);
        }
    }
}

void MediaPlayerView::slotCapture()
{
    if (d->player->playbackState() != QMediaPlayer::StoppedState)
    {
        int capturePosition    = d->player->position();
        QVideoSink* const sink = d->player->videoSink();
        QVideoFrame frame      = sink->videoFrame();
        QImage image           = frame.toImage();

        if (!image.isNull() && d->currentItem.isValid())
        {
            QFileInfo info(d->currentItem.toLocalFile());
            QString tempPath = QString::fromUtf8("%1/%2-%3.digikamtempfile.jpg")
                              .arg(info.path())
                              .arg(info.baseName())
                              .arg(capturePosition);

            if (image.save(tempPath, "JPG", 100))
            {
                QScopedPointer<DMetadata> meta(new DMetadata);

                if (meta->load(tempPath))
                {
                    QDateTime dateTime;
                    MetaEngine::ImageOrientation orientation = MetaEngine::ORIENTATION_NORMAL;

                    if (d->iface)
                    {
                        DItemInfo dinfo(d->iface->itemInfo(d->currentItem));

                        dateTime    = dinfo.dateTime();
                        orientation = (MetaEngine::ImageOrientation)dinfo.orientation();
                    }
                    else
                    {
                        QScopedPointer<DMetadata> meta2(new DMetadata);

                        if (meta2->load(d->currentItem.toLocalFile()))
                        {
                            dateTime    = meta2->getItemDateTime();
                            orientation = meta2->getItemOrientation();
                        }
                    }

                    if (dateTime.isValid())
                    {
                        dateTime = dateTime.addMSecs(capturePosition);
                    }
                    else
                    {
                        dateTime = QDateTime::currentDateTime();
                    }

                    if (orientation == MetaEngine::ORIENTATION_UNSPECIFIED)
                    {
                        orientation = MetaEngine::ORIENTATION_NORMAL;
                    }

                    meta->setImageDateTime(dateTime, true);
                    meta->setItemDimensions(image.size());
                    meta->setItemOrientation(orientation);
                    meta->save(tempPath, true);
                }

                QString finalPath = QString::fromUtf8("%1/%2-%3.jpg")
                                   .arg(info.path())
                                   .arg(info.baseName())
                                   .arg(capturePosition);

                if (QFile::rename(tempPath, finalPath))
                {
                    if (d->iface)
                    {
                        d->iface->slotMetadataChangedForUrl(QUrl::fromLocalFile(finalPath));
                    }
                }
                else
                {
                    QFile::remove(tempPath);
                }
            }
        }
    }
}

int MediaPlayerView::previewMode()
{
    return indexOf(currentWidget());
}

void MediaPlayerView::setPreviewMode(int mode)
{
    if ((mode != Private::ErrorView) && (mode != Private::PlayerView))
    {
        return;
    }

    setCurrentIndex(mode);

    d->toolBar->adjustSize();
    d->toolBar->raise();
}

void MediaPlayerView::setCurrentItem(const QUrl& url, bool hasPrevious, bool hasNext)
{
    d->prevAction->setEnabled(hasPrevious);
    d->nextAction->setEnabled(hasNext);
    d->adjustVideoSize();

    if (url.isEmpty())
    {
        d->player->stop();
        d->currentItem = url;

        return;
    }

    if (
        (d->currentItem == url) &&
        (
         (d->player->playbackState() == QMediaPlayer::PlayingState) ||
         (d->player->playbackState() == QMediaPlayer::PausedState)
        )
       )
    {
        return;
    }

    d->player->stop();
    int orientation = 0;
    d->currentItem  = url;

    if (d->iface)
    {
        DItemInfo info(d->iface->itemInfo(url));

        orientation = info.orientation();
    }

    switch (orientation)
    {
        case MetaEngine::ORIENTATION_ROT_90:
        case MetaEngine::ORIENTATION_ROT_90_HFLIP:
        case MetaEngine::ORIENTATION_ROT_90_VFLIP:
        {
            d->videoOrientation = 90;
            break;
        }

        case MetaEngine::ORIENTATION_ROT_180:
        {
            d->videoOrientation = 180;
            break;
        }

        case MetaEngine::ORIENTATION_ROT_270:
        {
            d->videoOrientation = 270;
            break;
        }

        default:
        {
            d->videoOrientation = 0;
            break;
        }
    }

    d->player->setSource(d->currentItem);
    setPreviewMode(Private::PlayerView);
    d->player->play();

    qCDebug(DIGIKAM_GENERAL_LOG) << "Play video with QtMultimedia started:" << d->player->source();
}

void MediaPlayerView::slotPositionChanged(qint64 position)
{
    if (
        (d->sliderTime < position)       &&
        ((d->sliderTime + 100) > position)
       )
    {
        return;
    }

    d->sliderTime = position;

    if (!d->slider->isSliderDown())
    {
        d->slider->blockSignals(true);
        d->slider->setValue(position);
        d->slider->blockSignals(false);
    }

    d->tlabel->setText(QString::fromLatin1("%1 / %2")
                       .arg(QTime(0, 0, 0).addMSecs(position).toString(QLatin1String("HH:mm:ss")))
                       .arg(QTime(0, 0, 0).addMSecs(d->slider->maximum()).toString(QLatin1String("HH:mm:ss"))));
}

void MediaPlayerView::slotVolumeChanged(int volume)
{
    d->audio->setVolume(volume / 100.0F);

    if (objectName() != QLatin1String("main_media_player"))
    {
        return;
    }

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Media Player Settings"));
    group.writeEntry("Volume", volume);
}

void MediaPlayerView::slotLoopToggled(bool loop)
{
    if (loop)
    {
        d->loopPlay->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-repeat")));
        d->player->setLoops(QMediaPlayer::Infinite);
    }
    else
    {
        d->loopPlay->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
        d->player->setLoops(QMediaPlayer::Once);
    }
}

void MediaPlayerView::slotDurationChanged(qint64 duration)
{
    qint64 max = qMax((qint64)1, duration);
    d->slider->setRange(0, max);
}

void MediaPlayerView::slotPlaybackRate(QAction* action)
{
    if (action)
    {
        d->player->setPlaybackRate(action->data().toReal());
    }
}

void MediaPlayerView::slotPosition(int position)
{
    if (d->player->isSeekable())
    {
        d->player->setPosition((qint64)position);
    }
}

void MediaPlayerView::slotHandlePlayerError(QMediaPlayer::Error /*error*/, const QString& errStr)
{
    setPreviewMode(Private::ErrorView);

    qCDebug(DIGIKAM_GENERAL_LOG) << "QtMultimedia Error: " << errStr;
}

void MediaPlayerView::slotNativeSizeChanged()
{
    d->adjustVideoSize();
}

bool MediaPlayerView::eventFilter(QObject* watched, QEvent* event)
{
    if ((watched == d->playerView) && (event->type() == QEvent::Resize))
    {
        d->adjustVideoSize();
    }

    return QStackedWidget::eventFilter(watched, event);
}

}  // namespace Digikam

#include "mediaplayerview.moc"

#include "moc_mediaplayerview.cpp"
