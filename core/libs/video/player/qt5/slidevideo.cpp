/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-22
 * Description : Slideshow video viewer
 *
 * SPDX-FileCopyrightText: 2014-2020 Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "slidevideo.h"

// Qt includes

#include <QApplication>
#include <QProxyStyle>
#include <QGridLayout>
#include <QWidget>
#include <QString>
#include <QSlider>
#include <QStyle>
#include <QLabel>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "metaengine.h"
#include "WidgetRenderer.h"
#include "QtAV.h"
#include "QtAV_Version.h"
#include "AVPlayerConfigMngr.h"
#include "DecoderConfigPage.h"

using namespace QtAV;

namespace Digikam
{

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

    explicit Private()
      : iface           (nullptr),
        videoWidget     (nullptr),
        player          (nullptr),
        slider          (nullptr),
        volume          (nullptr),
        tlabel          (nullptr),
        indicator       (nullptr),
        videoOrientation(0)
    {
    }

    DInfoInterface*      iface;

    WidgetRenderer*      videoWidget;
    AVPlayerCore*        player;

    QSlider*             slider;
    QSlider*             volume;
    QLabel*              tlabel;

    DHBox*               indicator;

    int                  videoOrientation;
};

SlideVideo::SlideVideo(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setMouseTracking(true);

    d->videoWidget    = new WidgetRenderer(this);
    d->videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->videoWidget->setOutAspectRatioMode(QtAV::VideoAspectRatio);
    d->videoWidget->setMouseTracking(true);

    d->player         = new AVPlayerCore(this);
    d->player->setRenderer(d->videoWidget);

    d->player->setBufferMode(QtAV::BufferPackets);
    d->player->setBufferValue(AVPlayerConfigMngr::instance().bufferValue());
    d->player->setFrameRate(AVPlayerConfigMngr::instance().forceFrameRate());
    d->player->setInterruptOnTimeout(AVPlayerConfigMngr::instance().abortOnTimeout());
    d->player->setInterruptTimeout(AVPlayerConfigMngr::instance().timeout() * 1000.0);
    d->player->setPriority(DecoderConfigPage::idsFromNames(AVPlayerConfigMngr::instance().decoderPriorityNames()));

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
    KConfigGroup group        = config->group("Media Player Settings");
    int volume                = group.readEntry("Volume", 50);

    d->player->audio()->setVolume((qreal)volume / 100.0);
    d->volume->setValue(volume);

    // --------------------------------------------------------------------------

    connect(d->slider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotPosition(int)));

    connect(d->slider, SIGNAL(valueChanged(int)),
            this, SLOT(slotPosition(int)));

    connect(d->volume, SIGNAL(valueChanged(int)),
            this, SLOT(slotVolumeChanged(int)));

    connect(d->player, SIGNAL(stateChanged(QtAV::AVPlayerCore::State)),
            this, SLOT(slotPlayerStateChanged(QtAV::AVPlayerCore::State)));

    connect(d->player, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),
            this, SLOT(slotMediaStatusChanged(QtAV::MediaStatus)));

    connect(d->player, SIGNAL(positionChanged(qint64)),
            this, SLOT(slotPositionChanged(qint64)));

    connect(d->player, SIGNAL(durationChanged(qint64)),
            this, SLOT(slotDurationChanged(qint64)));

    connect(d->player, SIGNAL(error(QtAV::AVError)),
            this, SLOT(slotHandlePlayerError(QtAV::AVError)));

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
            d->videoOrientation = 90;
            break;

        case MetaEngine::ORIENTATION_ROT_180:
            d->videoOrientation = 180;
            break;

        case MetaEngine::ORIENTATION_ROT_270:
            d->videoOrientation = 270;
            break;

        default:
            d->videoOrientation = 0;
            break;
    }

    d->player->setFile(url.toLocalFile());
    d->player->play();

    showIndicator(false);
}

void SlideVideo::showIndicator(bool b)
{
    d->indicator->setVisible(b);
}

void SlideVideo::slotPlayerStateChanged(QtAV::AVPlayerCore::State state)
{
    if (state == QtAV::AVPlayerCore::PlayingState)
    {
        int rotate = 0;

#if QTAV_VERSION > QTAV_VERSION_CHK(1, 12, 0)

        // fix wrong rotation from QtAV git/master

        rotate     = d->player->statistics().video_only.rotate;

#endif
        d->videoWidget->setOrientation((-rotate) + d->videoOrientation);
        qCDebug(DIGIKAM_GENERAL_LOG) << "Found video orientation:"
                                     << d->videoOrientation;
    }
}

void SlideVideo::slotMediaStatusChanged(QtAV::MediaStatus status)
{
    switch (status)
    {
        case EndOfMedia:
            Q_EMIT signalVideoFinished();
            break;

        case LoadedMedia:
            Q_EMIT signalVideoLoaded(true);
            break;

        case InvalidMedia:
            Q_EMIT signalVideoLoaded(false);
            break;

        default:
            break;
    }
}

void SlideVideo::pause(bool b)
{
    if (!b && !d->player->isPlaying())
    {
        d->player->play();
        return;
    }

    d->player->pause(b);
}

void SlideVideo::stop()
{
    d->player->stop();
    d->player->setFile(QString());
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
    d->player->audio()->setVolume((qreal)volume / 100.0);
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
        d->player->setPosition((qint64)position);
    }
}

void SlideVideo::slotHandlePlayerError(const QtAV::AVError& err)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Error: " << err.string();
}

} // namespace Digikam

#include "slidevideo.moc"
