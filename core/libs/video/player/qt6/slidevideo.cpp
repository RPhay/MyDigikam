/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-22
 * Description : Slideshow video viewer based on QtMultimedia
 *
 * SPDX-FileCopyrightText: 2014-2023 Gilles Caulier <caulier dot gilles at gmail dot com>
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

    QGraphicsScene*      videoScene       = nullptr;
    QGraphicsView*       videoView        = nullptr;
    QGraphicsVideoItem*  videoWidget      = nullptr;
    QMediaPlayer*        player           = nullptr;

    QSlider*             slider           = nullptr;
    QSlider*             volume           = nullptr;
    QLabel*              tlabel           = nullptr;

    DHBox*               indicator        = nullptr;

    int                  videoOrientation = 0;

public:

    void adjustVideoSize()
    {
        videoView->fitInView(videoWidget, Qt::KeepAspectRatio);
        videoView->centerOn(0, 0);
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
        qreal x          = videoWidget->boundingRect().width()  / 2.0;
        qreal y          = videoWidget->boundingRect().height() / 2.0;
        QTransform tfm;
        tfm.translate(x, y);
        tfm.rotate(orientation);
        tfm.translate(-x, -y);
        videoWidget->setTransform(tfm);
        videoOrientation = orientation;
        adjustVideoSize();
    };
};

SlideVideo::SlideVideo(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMouseTracking(true);

    d->videoScene  = new QGraphicsScene(this);
    d->videoView   = new QGraphicsView(d->videoScene);
    d->videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->videoView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    d->videoWidget = new QGraphicsVideoItem();
    d->player      = new QMediaPlayer(this);
    d->player->setVideoOutput(d->videoWidget);
    d->videoScene->addItem(d->videoWidget);

    d->videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    d->videoView->setMouseTracking(true);

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

    const int spacing        = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                                    QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    QVBoxLayout* const vbox2 = new QVBoxLayout(this);
    vbox2->addWidget(d->indicator,  0);
    vbox2->addWidget(d->videoView, 10);
    vbox2->setContentsMargins(0, 0, 0, 0);
    vbox2->setSpacing(spacing);

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group("Media Player Settings");
    int volume                = group.readEntry("Volume", 50);

    d->volume->setValue(volume);

    // --------------------------------------------------------------------------

    connect(d->slider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotPosition(int)));

    connect(d->slider, SIGNAL(valueChanged(int)),
            this, SLOT(slotPosition(int)));

    connect(d->volume, SIGNAL(valueChanged(int)),
            this, SLOT(slotVolumeChanged(int)));

    connect(d->player, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)),
            this, SLOT(slotPlayerStateChanged(QMediaPlayer::PlaybackState)));

    connect(d->player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(slotMediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(d->player, SIGNAL(positionChanged(qint64)),
            this, SLOT(slotPositionChanged(qint64)));

    connect(d->player, SIGNAL(durationChanged(qint64)),
            this, SLOT(slotDurationChanged(qint64)));

    connect(d->player, SIGNAL(errorOccurred(QMediaPlayer::Error,QString)),
            this, SLOT(slotHandlePlayerError(QMediaPlayer::Error,QString)));
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

    d->player->setSource(url);
    d->player->play();

    qCDebug(DIGIKAM_GENERAL_LOG) << "Slide video with QtMultimedia started:" << d->player->source();

    showIndicator(false);
}

void SlideVideo::showIndicator(bool b)
{
    d->indicator->setVisible(b);
}

void SlideVideo::slotPlayerStateChanged(QMediaPlayer::PlaybackState newState)
{
    if (newState == QMediaPlayer::PlayingState)
    {
        int rotate = d->videoMediaOrientation();
        d->setVideoItemOrientation((-rotate) + d->videoOrientation);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Found video orientation with QtMultimedia:"
                                     << d->videoOrientation;
    }
}

void SlideVideo::slotMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status)
    {
        case QMediaPlayer::EndOfMedia:
            qCDebug(DIGIKAM_GENERAL_LOG) << "Slide video with QtMultimedia completed:" << d->player->source();

            Q_EMIT signalVideoFinished();

            break;

        case QMediaPlayer::LoadedMedia:

            Q_EMIT signalVideoLoaded(true);

            break;

        case QMediaPlayer::InvalidMedia:

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

    d->player->pause();
}

void SlideVideo::stop()
{
    d->player->stop();
    d->player->setSource(QUrl());
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
    d->player->audioOutput()->setVolume(volume / 100.0F);
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

void SlideVideo::slotHandlePlayerError(QMediaPlayer::Error, const QString& str)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "QtMultimedia Error: " << str;
}

void SlideVideo::resizeEvent(QResizeEvent*)
{
    d->adjustVideoSize();
}

} // namespace Digikam

#include "slidevideo.moc"
