/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-22
 * Description : Slideshow video viewer
 *
 * Copyright (C) 2014-2020 Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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

// QtAV includes

#include <QtAV/VideoOutput.h>    // krazy:exclude=includes
#include <QtAV/LibAVFilter.h>    // krazy:exclude=includes
#include <QtAV/version.h>        // krazy:exclude=includes

// Local includes

#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "metaengine.h"

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
            return (Qt::LeftButton | Qt::MidButton | Qt::RightButton);
        }

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

class Q_DECL_HIDDEN SlideVideo::Private
{

public:

    explicit Private()
      : iface           (nullptr),
        rotateFilter    (nullptr),
        videoOutput     (nullptr),
        player          (nullptr),
        slider          (nullptr),
        volume          (nullptr),
        tlabel          (nullptr),
        indicator       (nullptr),
        sliderTime      (0)
    {
    }

    DInfoInterface*      iface;

    LibAVFilterVideo*    rotateFilter;
    VideoOutput*         videoOutput;
    AVPlayer*            player;

    QSlider*             slider;
    QSlider*             volume;
    QLabel*              tlabel;

    DHBox*               indicator;

    qint64               sliderTime;
};

SlideVideo::SlideVideo(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);

    d->videoOutput         = new VideoOutput(this);
    QWidget* const vWidget = d->videoOutput->widget() ? d->videoOutput->widget()
                                                      : new QWidget(this);

    d->player              = new AVPlayer(this);
    d->rotateFilter        = new LibAVFilterVideo(d->player);

    d->videoOutput->setOutAspectRatioMode(VideoRenderer::VideoAspectRatio);
    d->player->setRenderer(d->videoOutput);
    vWidget->setMouseTracking(true);

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
    grid->addWidget(vWidget,      0, 0, 2, 1);
    grid->addWidget(d->indicator, 0, 0, 1, 1); // Widget will be over player to not change layout when visibility is changed.
    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 100);
    grid->setContentsMargins(QMargins());

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group = config->group("Media Player Settings");
    int volume         = group.readEntry("Volume", 50);

    d->volume->setValue(volume);
    d->player->audio()->setVolume((qreal)volume / 100.0);

    // --------------------------------------------------------------------------

    connect(d->slider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotPosition(int)));

    connect(d->slider, SIGNAL(valueChanged(int)),
            this, SLOT(slotPosition(int)));

    connect(d->volume, SIGNAL(valueChanged(int)),
            this, SLOT(slotVolumeChanged(int)));

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

    int videoAngle      = 0;
    int orientation     = 0;
    bool supportedCodec = true;

    if (d->iface)
    {
        DItemInfo info(d->iface->itemInfo(url));

        orientation = info.orientation();

        if (info.videoCodec() == QLatin1String("none"))
        {
            supportedCodec = false;
        }
    }

    switch (orientation)
    {
        case MetaEngine::ORIENTATION_ROT_90:
        case MetaEngine::ORIENTATION_ROT_90_HFLIP:
        case MetaEngine::ORIENTATION_ROT_90_VFLIP:
            videoAngle = 90;
            break;

        case MetaEngine::ORIENTATION_ROT_180:
            videoAngle = 180;
            break;

        case MetaEngine::ORIENTATION_ROT_270:
            videoAngle = 270;
            break;

        default:
            videoAngle = 0;
            break;
    }

    d->rotateFilter->uninstall();

    if (videoAngle != 0)
    {
        d->rotateFilter->installTo(d->player);
        d->rotateFilter->setOptions(QString::fromLatin1("rotate=PI*%1").arg(videoAngle));
    }

    if (supportedCodec)
    {
        d->player->setFile(url.toLocalFile());
        d->player->play();
    }
    else
    {
        d->player->setFile(QString());
        emit signalVideoLoaded(false);
    }

    showIndicator(false);
}

void SlideVideo::showIndicator(bool b)
{
    d->indicator->setVisible(b);
    d->indicator->raise();
}

void SlideVideo::slotMediaStatusChanged(QtAV::MediaStatus status)
{
    switch (status)
    {
        case EndOfMedia:
            emit signalVideoFinished();
            break;

        case LoadedMedia:
            emit signalVideoLoaded(true);
            break;

        case InvalidMedia:
            emit signalVideoLoaded(false);
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

    emit signalVideoPosition(position);
}

void SlideVideo::slotVolumeChanged(int volume)
{
    d->player->audio()->setVolume((qreal)volume / 100.0);
}

void SlideVideo::slotDurationChanged(qint64 duration)
{
    qint64 max = qMax((qint64)1, duration);
    d->slider->setRange(0, max);

    emit signalVideoDuration(duration);
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
