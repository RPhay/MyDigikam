/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-20-12
 * Description : a view to host media player based on QtAVPlayer.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QStyle>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QMediaService>
#include <QMediaObject>
#include <QVideoRendererControl>
#include <QVideoWidget>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// QtAVPlayer includes

#include "qavvideoframe.h"
#include "qavaudiooutput.h"

// Local includes

#include "digikam_globals.h"
#include "digikam_debug.h"
#include "thememanager.h"
#include "dlayoutbox.h"
#include "metaengine.h"
#include "dmetadata.h"

namespace Digikam
{

class Q_DECL_HIDDEN VideoRenderer : public QVideoRendererControl
{
public:

    VideoRenderer(QObject* const parent = nullptr)
        : QVideoRendererControl(parent)
    {
    }

    QAbstractVideoSurface* surface() const override
    {
        return m_surface;
    }

    void setSurface(QAbstractVideoSurface* surface) override
    {
        m_surface = surface;
    }

    QAbstractVideoSurface* m_surface = nullptr;
};

// --------------------------------------------------------

class Q_DECL_HIDDEN MediaService : public QMediaService
{
public:

    MediaService(VideoRenderer* const vr, QObject* const parent = nullptr)
        : QMediaService(parent),
          m_renderer   (vr)
    {
    }

    QMediaControl* requestControl(const char* name) override
    {
        if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        {
            return m_renderer;
        }

        return nullptr;
    }

    void releaseControl(QMediaControl*) override
    {
    }

    VideoRenderer* m_renderer = nullptr;
};

// --------------------------------------------------------

class Q_DECL_HIDDEN MediaObject : public QMediaObject
{
public:

    explicit MediaObject(VideoRenderer* const vr, QObject* const parent = nullptr)
        : QMediaObject(parent, new MediaService(vr, parent))
    {
    }
};

// --------------------------------------------------------

class Q_DECL_HIDDEN VideoWidget : public QVideoWidget
{
public:

    VideoWidget(QWidget* const parent = nullptr)
        : QVideoWidget(parent)
    {
    }

    bool setMediaObject(QMediaObject* object) override
    {
        return QVideoWidget::setMediaObject(object);
    }
};

// --------------------------------------------------------

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
        if ((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::MouseButtonDblClick))
        {
            bool singleClick              = qApp->style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick);
            QMouseEvent* const mouseEvent = dynamic_cast<QMouseEvent*>(event);

            if (m_parent && mouseEvent)
            {
                MediaPlayerView* const mplayer = dynamic_cast<MediaPlayerView*>(m_parent);

                if (mplayer)
                {
                    if      ((mouseEvent->button() == Qt::LeftButton) &&
                             ((singleClick && (event->type() == QEvent::MouseButtonRelease)) ||
                             (!singleClick && (event->type() == QEvent::MouseButtonDblClick))))
                    {
                        mplayer->slotEscapePressed();
                    }
                    else if ((mouseEvent->button() == Qt::RightButton) &&
                             (event->type() == QEvent::MouseButtonRelease))
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

class Q_DECL_HIDDEN MediaPlayerView::Private
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

    VideoRenderer*       videoRender        = nullptr;
    VideoWidget*         videoWidget        = nullptr;
    MediaObject*         mediaObject        = nullptr;
    QAVPlayer*           player             = nullptr;
    QAVAudioOutput*      audioOutput        = nullptr;

    QSlider*             slider             = nullptr;
    QSlider*             volume             = nullptr;
    QLabel*              tlabel             = nullptr;
    QUrl                 currentItem;

    int                  videoOrientation   = 0;
    qint64               capturePosition    = 0;
    qint64               sliderTime         = 0;
};

MediaPlayerView::MediaPlayerView(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
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

    d->playerView     = new QFrame(this);
    d->playerView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->playerView->setLineWidth(1);

    d->videoRender    = new VideoRenderer(this);

    d->videoWidget    = new VideoWidget(this);
    d->videoWidget->setMouseTracking(true);

    d->mediaObject    = new MediaObject(d->videoRender);
    d->videoWidget->setMediaObject(d->mediaObject);

    d->player         = new QAVPlayer(this);

    d->audioOutput    = new QAVAudioOutput(this);

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
    QLabel* const spk = new QLabel(hbox);
    spk->setPixmap(QIcon::fromTheme(QLatin1String("audio-volume-high")).pixmap(22, 22));
    d->volume         = new QSlider(Qt::Horizontal, hbox);
    d->volume->setRange(0, 100);
    d->volume->setValue(50);

    hbox->setStretchFactor(d->slider, 10);
    hbox->setContentsMargins(0, 0, 0, spacing);
    hbox->setSpacing(spacing);

    QVBoxLayout* const vbox2 = new QVBoxLayout(d->playerView);
    vbox2->addWidget(d->videoWidget, 10);
    vbox2->addWidget(hbox,            0);
    vbox2->setContentsMargins(QMargins());
    vbox2->setSpacing(spacing);

    insertWidget(Private::PlayerView, d->playerView);

    d->toolBar = new QToolBar(this);
    d->toolBar->addAction(d->prevAction);
    d->toolBar->addAction(d->nextAction);
    d->toolBar->addAction(d->playAction);
    d->toolBar->addAction(d->grabAction);
    d->toolBar->setStyleSheet(toolButtonStyleSheet());

    setPreviewMode(Private::PlayerView);

    d->errorView->installEventFilter(new MediaPlayerMouseClickFilter(this));
    d->videoWidget->installEventFilter(new MediaPlayerMouseClickFilter(this));

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Media Player Settings"));
    int volume                = group.readEntry("Volume", 50);

    d->audioOutput->setVolume(volume);
    d->volume->setValue(volume);

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

    connect(d->player, &QAVPlayer::audioFrame,
            this, &MediaPlayerView::slotAudioFrame,
            Qt::DirectConnection);

    connect(d->player, &QAVPlayer::videoFrame,
            this, &MediaPlayerView::slotVideoFrame,
            Qt::DirectConnection);

    connect(d->player, SIGNAL(stateChanged(QAVPlayer::State)),
            this, SLOT(slotPlayerStateChanged(QAVPlayer::State)));

    connect(d->player, SIGNAL(seeked(qint64)),
            this, SLOT(slotPositionChanged(qint64)),
            Qt::QueuedConnection);

    connect(d->player, SIGNAL(durationChanged(qint64)),
            this, SLOT(slotDurationChanged(qint64)));

    connect(d->player, SIGNAL(errorOccurred(QAVPlayer::Error, const QString&)),
            this, SLOT(slotHandlePlayerError(QAVPlayer::Error)));

    connect(d->player, SIGNAL(mediaStatusChanged(QAVPlayer::MediaStatus)),
            this, SLOT(slotMediaStatusChanged(QAVPlayer::MediaStatus)));
/*FIXME
    connect(d->player->videoCapture(), SIGNAL(imageCaptured(QImage)),
            this, SLOT(slotImageCaptured(QImage)));
*/
}

MediaPlayerView::~MediaPlayerView()
{
    escapePreview();
    delete d;
}

void MediaPlayerView::slotAudioFrame(const QAVAudioFrame& frame)
{
    d->audioOutput->play(frame);
}

void MediaPlayerView::slotVideoFrame(const QAVVideoFrame& frame)
{
    if (d->videoRender->m_surface == nullptr)
    {
        return;
    }

    QVideoFrame videoFrame = frame.convertTo(AV_PIX_FMT_RGB32);

    if (!d->videoRender->m_surface->isActive() || (d->videoRender->m_surface->surfaceFormat().frameSize() != videoFrame.size()))
    {
        QVideoSurfaceFormat f(videoFrame.size(), videoFrame.pixelFormat(), videoFrame.handleType());
        d->videoRender->m_surface->start(f);
    }

    if (d->videoRender->m_surface->isActive())
    {
         d->videoRender->m_surface->present(videoFrame);
    }
}

void MediaPlayerView::setInfoInterface(DInfoInterface* const iface)
{
    d->iface = iface;
}

void MediaPlayerView::reload()
{
    d->player->stop();
    d->player->setSource(d->currentItem.toLocalFile());
    d->player->play();
}

void MediaPlayerView::slotPlayerStateChanged(QAVPlayer::State newState)
{
    if      (newState == QAVPlayer::PlayingState)
    {
/*FIXME
        int rotate = 0;

        // fix wrong rotation from QtAV git/master

        rotate     = d->player->statistics().video_only.rotate;

        d->videoWidget->setOrientation((-rotate) + d->videoOrientation);
        qCDebug(DIGIKAM_GENERAL_LOG) << "Found video orientation with QtAV:"
                                     << d->videoOrientation;
*/
        d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));

    }
    else if ((newState == QAVPlayer::PausedState) ||
             (newState == QAVPlayer::StoppedState))
    {
        d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    }
}

void MediaPlayerView::slotMediaStatusChanged(QAVPlayer::MediaStatus newStatus)
{
    if (newStatus == QAVPlayer::InvalidMedia)
    {
        setPreviewMode(Private::ErrorView);
    }
}

void MediaPlayerView::escapePreview()
{
    d->player->stop();
    d->player->setSource(QString());
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
/*FIXME
    if (d->player->isPlaying())
    {
        int orientation = 0;

        switch (d->videoWidget->orientation())
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

        d->videoWidget->setOrientation(orientation);
    }
*/
}

void MediaPlayerView::slotPausePlay()
{
    if (d->player->state() != QAVPlayer::PlayingState)
    {
        d->player->play();
        return;
    }

    if (d->player->state() != QAVPlayer::PausedState)
    {
       d->player->pause();
    }
}

void MediaPlayerView::slotCapture()
{
    if (d->player->state() == QAVPlayer::PlayingState)
    {
/*FIXME
        d->player->videoCapture()->setAutoSave(false);
        d->capturePosition = d->player->position();
        d->player->videoCapture()->capture();
*/
    }
}

void MediaPlayerView::slotImageCaptured(const QImage& image)
{
/*FIXME
    if (!image.isNull() && d->currentItem.isValid())
    {
        QFileInfo info(d->currentItem.toLocalFile());
        QString tempPath = QString::fromUtf8("%1/%2-%3.digikamtempfile.jpg")
                          .arg(info.path())
                          .arg(info.baseName())
                          .arg(d->capturePosition);

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
                    dateTime = dateTime.addMSecs(d->capturePosition);
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
                               .arg(d->capturePosition);

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
*/
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

    if (url.isEmpty())
    {
        d->player->stop();
        d->currentItem = url;
/*FIXME?
        d->player->audio()->close();
*/
        return;
    }

    if (d->currentItem == url)
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

    d->player->setSource(d->currentItem.toLocalFile());
    setPreviewMode(Private::PlayerView);
    d->player->seek(10);
    d->player->play();
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
    d->audioOutput->setVolume((qreal)volume / 100.0);

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
/*FIXME
    if (loop)
    {
        d->loopPlay->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-repeat")));
        d->player->setRepeat(-1);
    }
    else
    {
        d->loopPlay->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
        d->player->setRepeat(0);
    }
*/
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
        d->player->seek((qint64)position);
    }
}

void MediaPlayerView::slotHandlePlayerError(QAVPlayer::Error err)
{
    setPreviewMode(Private::ErrorView);
    qCDebug(DIGIKAM_GENERAL_LOG) << "QtAVPlayer Error: " << err;
}

} // namespace Digikam

#include "mediaplayerview.moc"

#include "moc_mediaplayerview.cpp"
