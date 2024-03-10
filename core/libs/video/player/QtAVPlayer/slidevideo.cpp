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
#include <QTimer>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "metaengine.h"

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

    Private() = default;

    DInfoInterface*      iface            = nullptr;

    DVideoWidget*        videoWidget      = nullptr;

    QSlider*             slider           = nullptr;
    QSlider*             volume           = nullptr;
    QLabel*              tlabel           = nullptr;

    DHBox*               indicator        = nullptr;
};

SlideVideo::SlideVideo(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setMouseTracking(true);

    d->videoWidget    = new DVideoWidget(this);

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

    d->videoWidget->audioOutput()->setVolume(volume);
    d->volume->setValue(volume);

    // --------------------------------------------------------------------------

    connect(d->slider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotPosition(int)));

    connect(d->slider, SIGNAL(valueChanged(int)),
            this, SLOT(slotPosition(int)));

    connect(d->volume, SIGNAL(valueChanged(int)),
            this, SLOT(slotVolumeChanged(int)));

    connect(d->videoWidget->player(), SIGNAL(stateChanged(QAVPlayer::State)),
            this, SLOT(slotPlayerStateChanged(QAVPlayer::State)));

    connect(d->videoWidget->player(), SIGNAL(seeked(qint64)),
            this, SLOT(slotPositionChanged(qint64)),
            Qt::QueuedConnection);

    connect(d->videoWidget->player(), SIGNAL(durationChanged(qint64)),
            this, SLOT(slotDurationChanged(qint64)));

    connect(d->videoWidget->player(), SIGNAL(errorOccurred(QAVPlayer::Error,QString)),
            this, SLOT(slotHandlePlayerError(QAVPlayer::Error,QString)));

    connect(d->videoWidget->player(), SIGNAL(mediaStatusChanged(QAVPlayer::MediaStatus)),
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

void SlideVideo::setInfoInterface(DInfoInterface* const iface)
{
    d->iface = iface;
}

void SlideVideo::setCurrentUrl(const QUrl& url)
{
    d->videoWidget->player()->stop();

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
            d->videoWidget->setVideoItemOrientation(90);
            break;
        }

        case MetaEngine::ORIENTATION_ROT_180:
        {
            d->videoWidget->setVideoItemOrientation(180);
            break;
        }

        case MetaEngine::ORIENTATION_ROT_270:
        {
            d->videoWidget->setVideoItemOrientation(270);
            break;
        }

        default:
        {
            d->videoWidget->setVideoItemOrientation(0);
            break;
        }
    }

    d->videoWidget->player()->setSource(url.toLocalFile());
    d->videoWidget->player()->play();

    showIndicator(false);
}

void SlideVideo::showIndicator(bool b)
{
    d->indicator->setVisible(b);
}

void SlideVideo::slotPlayerStateChanged(QAVPlayer::State newState)
{
    if (newState == QAVPlayer::PlayingState)
    {
        // Nothing to do.
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
            slotPlayingStateChanged();

            Q_EMIT signalVideoLoaded(true);

            QTimer::singleShot(250, this, SLOT(slotPlayingStateChanged()));

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
    if (!b && (d->videoWidget->player()->state() != QAVPlayer::PlayingState))
    {
        d->videoWidget->player()->play();
        return;
    }

    if (b && (d->videoWidget->player()->state() != QAVPlayer::PausedState))
    {
       d->videoWidget->player()->pause();
    }
}

void SlideVideo::stop()
{
    d->videoWidget->player()->stop();
    d->videoWidget->player()->setSource(QString());
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
    d->videoWidget->audioOutput()->setVolume((qreal)volume / 100.0);
}

void SlideVideo::slotDurationChanged(qint64 duration)
{
    qint64 max = qMax((qint64)1, duration);
    d->slider->setRange(0, max);

    Q_EMIT signalVideoDuration(duration);
}

void SlideVideo::slotPosition(int position)
{
    if (d->videoWidget->player()->isSeekable())
    {
        d->videoWidget->player()->seek((qint64)position);
    }
}

void SlideVideo::slotHandlePlayerError(QAVPlayer::Error /*err*/, const QString& message)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "QtAVPlayer Error: " << message;
}

void SlideVideo::slotPlayingStateChanged()
{
    int rotate = d->videoWidget->videoMediaOrientation();

    qCDebug(DIGIKAM_GENERAL_LOG) << "Video orientation from QtAVPlayer:"
                                 << rotate;

    if (rotate != d->videoWidget->videoItemOrientation())
    {
        rotate = d->videoWidget->videoItemOrientation();
    }

    d->videoWidget->setVideoItemOrientation(rotate);
}

} // namespace Digikam

#include "slidevideo.moc"

#include "moc_slidevideo.cpp"
