/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-20-12
 * Description : a view to embed QtMultimedia media player.
 *
 * SPDX-FileCopyrightText: 2006-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "mediaplayerview.h"

// Qt includes

#include <QApplication>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QProxyStyle>
#include <QPushButton>
#include <QFileInfo>
#include <QToolBar>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QFrame>
#include <QEvent>
#include <QStyle>
#include <QTransform>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QVideoSink>
#include <QVideoFrame>
#include <QAudioOutput>
#include <QMediaMetaData>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "stackedview.h"
#include "thememanager.h"
#include "dlayoutbox.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
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
                    if      ((mouseEvent->button() == Qt::LeftButton) &&
                             ((singleClick && (event->type() == QEvent::MouseButtonPress)) ||
                             (!singleClick && (event->type() == QEvent::MouseButtonDblClick))))
                    {
                        mplayer->slotEscapePressed();
                    }
                    else if ((mouseEvent->button() == Qt::RightButton) &&
                             (event->type() == QEvent::MouseButtonPress))
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

    QPushButton*         loopPlay           = nullptr;

    QToolBar*            toolBar            = nullptr;

    DInfoInterface*      iface              = nullptr;

    QGraphicsScene*      videoScene         = nullptr;
    QGraphicsView*       videoView          = nullptr;
    QGraphicsVideoItem*  videoWidget        = nullptr;
    QMediaPlayer*        player             = nullptr;
    QAudioOutput*        audio              = nullptr;

    QSlider*             slider             = nullptr;
    QSlider*             volume             = nullptr;
    QLabel*              tlabel             = nullptr;
    QUrl                 currentItem;

    int                  videoOrientation   = 0;
    qint64               sliderTime         = 0;

public:

    void adjustVideoSize()
    {
        videoView->fitInView(videoWidget, Qt::KeepAspectRatio);
        videoView->centerOn(videoWidget);
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
        videoView->rotate(orientation);
        videoOrientation = orientation;
        adjustVideoSize();
    };
};

MediaPlayerView::MediaPlayerView(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    const int spacing      = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                                  QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    d->prevAction          = new QAction(QIcon::fromTheme(QLatin1String("go-previous")),
                                         i18nc("go to previous image", "Back"),   this);
    d->nextAction          = new QAction(QIcon::fromTheme(QLatin1String("go-next")),
                                         i18nc("go to next image", "Forward"),    this);
    d->playAction          = new QAction(QIcon::fromTheme(QLatin1String("media-playback-start")),
                                         i18nc("pause/play video", "Pause/Play"), this);
    d->grabAction          = new QAction(QIcon::fromTheme(QLatin1String("view-preview")),
                                         i18nc("capture video frame", "Capture"), this);

    d->toolBar             = new QToolBar(this);
    d->toolBar->addAction(d->prevAction);
    d->toolBar->addAction(d->nextAction);
    d->toolBar->addAction(d->playAction);
    d->toolBar->addAction(d->grabAction);
    d->toolBar->setStyleSheet(toolButtonStyleSheet());

    d->errorView           = new QFrame(this);
    QLabel* const errorMsg = new QLabel(i18n("An error has occurred with the media player..."), this);

    errorMsg->setAlignment(Qt::AlignCenter);
    d->errorView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->errorView->setLineWidth(1);

    QVBoxLayout* const vbox1 = new QVBoxLayout(d->errorView);
    vbox1->addWidget(d->toolBar, 1);
    vbox1->addWidget(errorMsg, 10);
    vbox1->setContentsMargins(QMargins());
    vbox1->setSpacing(spacing);

    insertWidget(Private::ErrorView, d->errorView);

    // --------------------------------------------------------------------------

    d->playerView  = new QFrame(this);
    d->videoScene  = new QGraphicsScene(this);
    d->videoView   = new QGraphicsView(d->videoScene);
    d->videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->videoView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    d->videoWidget = new QGraphicsVideoItem();
    d->player      = new QMediaPlayer(this);
    d->audio       = new QAudioOutput;
    d->player->setAudioOutput(d->audio);
    d->player->setVideoOutput(d->videoWidget);
    d->videoScene->addItem(d->videoWidget);

    DHBox* const hbox = new DHBox(this);
    hbox->layout()->addWidget(d->toolBar);
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
    QLabel* const spk = new QLabel(hbox);
    spk->setPixmap(QIcon::fromTheme(QLatin1String("audio-volume-high")).pixmap(22, 22));
    d->volume         = new QSlider(Qt::Horizontal, hbox);
    d->volume->setRange(0, 100);
    d->volume->setValue(50);
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->setStretchFactor(d->slider, 10);
    hbox->setSpacing(spacing);

    d->videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    d->videoView->setMouseTracking(true);

    d->playerView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->playerView->setLineWidth(1);

    QVBoxLayout* const vbox2 = new QVBoxLayout(d->playerView);
    vbox2->addWidget(hbox,          0);
    vbox2->addWidget(d->videoView, 10);
    vbox2->setContentsMargins(0, 0, 0, 0);
    vbox2->setSpacing(spacing);

    insertWidget(Private::PlayerView, d->playerView);

    setPreviewMode(Private::PlayerView);

    d->errorView->installEventFilter(new MediaPlayerMouseClickFilter(this));
    d->videoView->installEventFilter(new MediaPlayerMouseClickFilter(this));

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group("Media Player Settings");

    d->volume->setValue(group.readEntry("Volume", 50));

    // --------------------------------------------------------------------------

    connect(ThemeManager::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));

    connect(d->prevAction, SIGNAL(triggered()),
            this, SIGNAL(signalPrevItem()));

    connect(d->nextAction, SIGNAL(triggered()),
            this, SIGNAL(signalNextItem()));

    connect(d->playAction, SIGNAL(triggered()),
            this, SLOT(slotPausePlay()));

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
}

MediaPlayerView::~MediaPlayerView()
{
    escapePreview();

    d->player->stop();
    delete d->player;
    delete d->videoWidget;
    delete d->slider;

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
    d->adjustVideoSize();
    d->player->play();
}

void MediaPlayerView::slotPlayerStateChanged(QMediaPlayer::PlaybackState newState)
{
    if (newState == QMediaPlayer::PlayingState)
    {
        int rotate = d->videoMediaOrientation();
        d->setVideoItemOrientation((-rotate) + d->videoOrientation);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Found video orientation with QtMultimedia:"
                                     << d->videoOrientation;

        d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
    }
    else
    {
        d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));

        if (
            (newState           == QMediaPlayer::StoppedState)  &&
            (d->player->error() != QMediaPlayer::NoError)
           )
        {
            setPreviewMode(Private::ErrorView);
        }

        if (
            (newState                 == QMediaPlayer::StoppedState) &&
            (d->player->mediaStatus() == QMediaPlayer::EndOfMedia)
           )
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Play video with QtMultimedia completed:" << d->player->source();

            if (d->player->error() != QMediaPlayer::NoError)
            {
                setPreviewMode(Private::ErrorView);
            }
        }
    }
}

void MediaPlayerView::slotMediaStatusChanged(QMediaPlayer::MediaStatus newStatus)
{
    if (newStatus == QMediaPlayer::InvalidMedia)
    {
        setPreviewMode(Private::ErrorView);
    }
}

void MediaPlayerView::escapePreview()
{
    d->player->stop();
    d->player->setSource(QUrl());
}

void MediaPlayerView::slotThemeChanged()
{
    QPalette palette;
    palette.setColor(d->errorView->backgroundRole(), qApp->palette().color(QPalette::Base));
    d->errorView->setPalette(palette);

    QPalette palette2;
    palette2.setColor(d->playerView->backgroundRole(), qApp->palette().color(QPalette::Base));
    d->playerView->setPalette(palette2);
}

void MediaPlayerView::slotEscapePressed()
{
    escapePreview();

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

void MediaPlayerView::slotCapture()
{
    if (d->player->isPlaying())
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
    d->videoView->resetTransform();
    d->adjustVideoSize();

    if (url.isEmpty())
    {
        d->player->stop();
        d->currentItem = url;
        d->player->stop();

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

    d->currentItem = url;

    d->player->stop();

    int orientation = 0;

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
    d->player->setPosition(10);
    d->player->play();

    qCDebug(DIGIKAM_GENERAL_LOG) << "Play video with QtMultimedia started:" << d->player->source();
}

void MediaPlayerView::slotPositionChanged(qint64 position)
{
    if ((d->sliderTime < position)       &&
        ((d->sliderTime + 100) > position))
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
    KConfigGroup group        = config->group("Media Player Settings");
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

void MediaPlayerView::resizeEvent(QResizeEvent* e)
{
    QStackedWidget::resizeEvent(e);
    d->adjustVideoSize();
}

void MediaPlayerView::showEvent(QShowEvent* e)
{
    QStackedWidget::showEvent(e);
    d->adjustVideoSize();
}

}  // namespace Digikam

#include "mediaplayerview.moc"
