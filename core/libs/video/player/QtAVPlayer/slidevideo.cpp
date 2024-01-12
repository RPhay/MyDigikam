/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-22
 * Description : Slideshow video viewer based on QtAVPlayer
 *
 * SPDX-FileCopyrightText: 2014-2024 Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "slidevideo.h"

// Qt includes

#include <QApplication>
#include <QProxyStyle>
#include <QGridLayout>
#include <QString>
#include <QSlider>
#include <QStyle>
#include <QLabel>
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

#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "metaengine.h"

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

class Q_DECL_HIDDEN SlideVideoStyle : public QProxyStyle
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

class Q_DECL_HIDDEN SlideVideo::Private
{
public:

    Private() = default;

    DInfoInterface*      iface            = nullptr;

    VideoRenderer*       videoRender      = nullptr;
    VideoWidget*         videoWidget      = nullptr;
    MediaObject*         mediaObject      = nullptr;
    QAVPlayer*           player           = nullptr;
    QAVAudioOutput*      audioOutput      = nullptr;

    QSlider*             slider           = nullptr;
    QSlider*             volume           = nullptr;
    QLabel*              tlabel           = nullptr;

    DHBox*               indicator        = nullptr;

    int                  videoOrientation = 0;
};

SlideVideo::SlideVideo(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setMouseTracking(true);

    d->videoRender    = new VideoRenderer(this);

    d->videoWidget    = new VideoWidget(this);
    d->videoWidget->setMouseTracking(true);

    d->mediaObject    = new MediaObject(d->videoRender);
    d->videoWidget->setMediaObject(d->mediaObject);

    d->player         = new QAVPlayer(this);

    d->audioOutput    = new QAVAudioOutput(this);

    d->indicator      = new DHBox(this);
    d->slider         = new QSlider(Qt::Horizontal, d->indicator);
    d->slider->setStyle(new SlideVideoStyle());
    d->slider->setRange(0, 0);
    d->slider->setAutoFillBackground(true);
    d->tlabel         = new QLabel(d->indicator);
    d->tlabel->setText(QLatin1String("00:00:00 / 00:00:00"));
    d->tlabel->setAutoFillBackground(true);
    QLabel* const spk = new QLabel(d->indicator);
    spk->setPixmap(QIcon::fromTheme(QLatin1String("audio-volume-high")).pixmap(22, 22));
    d->volume         = new QSlider(Qt::Horizontal, d->indicator);
    d->volume->setRange(0, 100);
    d->volume->setValue(50);
    d->indicator->setStretchFactor(d->slider, 10);
    d->indicator->setAutoFillBackground(true);
    d->indicator->setSpacing(4);

    QGridLayout* const grid = new QGridLayout(this);
    grid->addWidget(d->videoWidget, 0, 0, 2, 1);
    grid->addWidget(d->indicator,   0, 0, 1, 1); // Widget will be over player to not change layout when visibility is changed.
    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 100);
    grid->setContentsMargins(QMargins());

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Media Player Settings"));
    int volume                = group.readEntry("Volume", 50);

    d->audioOutput->setVolume(volume);
    d->volume->setValue(volume);

    // --------------------------------------------------------------------------

    connect(d->slider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotPosition(int)));

    connect(d->slider, SIGNAL(valueChanged(int)),
            this, SLOT(slotPosition(int)));

    connect(d->volume, SIGNAL(valueChanged(int)),
            this, SLOT(slotVolumeChanged(int)));

    connect(d->player, &QAVPlayer::audioFrame,
            this, &SlideVideo::slotAudioFrame,
            Qt::DirectConnection);

    connect(d->player, &QAVPlayer::videoFrame,
            this, &SlideVideo::slotVideoFrame,
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

    // --------------------------------------------------------------------------

    layout()->activate();
    resize(sizeHint());
    show();
}

SlideVideo::~SlideVideo()
{
    stop();
    delete d;
}

void SlideVideo::slotAudioFrame(const QAVAudioFrame& frame)
{
    d->audioOutput->play(frame);
}

void SlideVideo::slotVideoFrame(const QAVVideoFrame& frame)
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

void SlideVideo::setInfoInterface(DInfoInterface* const iface)
{
    d->iface = iface;
}

void SlideVideo::setCurrentUrl(const QUrl& url)
{
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

    d->player->setSource(url.toLocalFile());
    d->player->play();

    showIndicator(false);
}

void SlideVideo::showIndicator(bool b)
{
    d->indicator->setVisible(b);
}

void SlideVideo::slotPlayerStateChanged(QAVPlayer::State newState)
{
    if      (newState == QAVPlayer::PlayingState)
    {
/*FIXME
        int rotate = 0;

#if QTAV_VERSION > QTAV_VERSION_CHK(1, 12, 0)

        // fix wrong rotation from QtAV git/master

        rotate     = d->player->statistics().video_only.rotate;

#endif
        d->videoWidget->setOrientation((-rotate) + d->videoOrientation);
        qCDebug(DIGIKAM_GENERAL_LOG) << "Found video orientation with QtAV:"
                                     << d->videoOrientation;
*/
    }
}

void SlideVideo::slotMediaStatusChanged(QAVPlayer::MediaStatus newStatus)
{
    switch (newStatus)
    {
        case QAVPlayer::EndOfMedia:
        {
            Q_EMIT signalVideoFinished();

            break;
        }

        case QAVPlayer::LoadedMedia:
        {
            Q_EMIT signalVideoLoaded(true);

            break;
        }

        case QAVPlayer::InvalidMedia:
        {
            Q_EMIT signalVideoLoaded(false);

            break;
        }

        default:
        {
            break;
        }
    }
}

void SlideVideo::pause(bool b)
{
    if (!b && (d->player->state() != QAVPlayer::PlayingState))
    {
        d->player->play();
        return;
    }

    if (b && (d->player->state() != QAVPlayer::PausedState))
    {
       d->player->pause();
    }
}

void SlideVideo::stop()
{
    d->player->stop();
    d->player->setSource(QString());
}

void SlideVideo::slotPositionChanged(qint64 position)
{
    if (!d->slider->isSliderDown())
    {
        d->slider->blockSignals(true);
        d->slider->setValue(position);
        d->slider->blockSignals(false);
    }

    d->tlabel->setText(QString::fromLatin1("%1 / %2")
                       .arg(QTime(0, 0, 0).addMSecs(position).toString(QLatin1String("HH:mm:ss")))
                       .arg(QTime(0, 0, 0).addMSecs(d->slider->maximum()).toString(QLatin1String("HH:mm:ss"))));

    Q_EMIT signalVideoPosition(position);
}

void SlideVideo::slotVolumeChanged(int volume)
{
    d->audioOutput->setVolume((qreal)volume / 100.0);
}

void SlideVideo::slotDurationChanged(qint64 duration)
{
    qint64 max = qMax((qint64)1, duration);
    d->slider->setRange(0, max);

    Q_EMIT signalVideoDuration(duration);
}

void SlideVideo::slotPosition(int position)
{
    if (d->player->isSeekable())
    {
        d->player->seek((qint64)position);
    }
}

void SlideVideo::slotHandlePlayerError(QAVPlayer::Error err)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "QtAVPlayer Error: " << err;
}

} // namespace Digikam

#include "slidevideo.moc"

#include "moc_slidevideo.cpp"
