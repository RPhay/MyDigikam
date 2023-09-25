/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-20-12
 * Description : a view to embed QtMultimedia media player.
 *
 * Copyright (C) 2006-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "mediaplayerview.h"

// Qt includes

#include <QApplication>
#include <QVideoWidget>
#include <QVideoSink>
#include <QVideoFrame>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QFileInfo>
#include <QToolBar>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QFrame>
#include <QEvent>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "stackedview.h"
#include "thememanager.h"
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

    bool eventFilter(QObject* obj, QEvent* event)
    {
        if (
            (qApp->style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick)  && (event->type() == QEvent::MouseButtonRelease)) ||
            (!qApp->style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick) && (event->type() == QEvent::MouseButtonDblClick))
           )
        {
            QMouseEvent* const mouseEvent = dynamic_cast<QMouseEvent*>(event);

            if (mouseEvent && mouseEvent->button() == Qt::LeftButton)
            {
                if (m_parent)
                {
                    MediaPlayerView* const mplayer = dynamic_cast<MediaPlayerView*>(m_parent);

                    if (mplayer)
                    {
                        mplayer->slotEscapePressed();
                    }
                }

                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return QObject::eventFilter(obj, event);
        }
    }

private:

    QObject* m_parent = nullptr;
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

    QFrame*              errorView   = nullptr;
    QFrame*              playerView  = nullptr;

    QAction*             prevAction  = nullptr;
    QAction*             nextAction  = nullptr;
    QAction*             playAction  = nullptr;
    QAction*             grabAction  = nullptr;

    QToolBar*            toolBar     = nullptr;

    DInfoInterface*      iface       = nullptr;

    QVideoWidget*        videoWidget = nullptr;
    QMediaPlayer*        player      = nullptr;
    QSlider*             slider      = nullptr;
    QUrl                 currentItem;
};

MediaPlayerView::MediaPlayerView(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    const int spacing = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));
    QMargins margins(spacing, 0, spacing, spacing);

    d->prevAction          = new QAction(QIcon::fromTheme(QLatin1String("go-previous")),
                                         i18nc("go to previous image", "Back"),   this);
    d->nextAction          = new QAction(QIcon::fromTheme(QLatin1String("go-next")),
                                         i18nc("go to next image", "Forward"),    this);
    d->playAction          = new QAction(QIcon::fromTheme(QLatin1String("media-playback-start")),
                                         i18nc("pause/play video", "Pause/Play"), this);
    d->grabAction          = new QAction(QIcon::fromTheme(QLatin1String("view-preview")),
                                         i18nc("capture video frame", "Capture"), this);

    d->errorView           = new QFrame(this);
    QLabel* const errorMsg = new QLabel(i18n("An error has occurred with the media player...."), this);

    errorMsg->setAlignment(Qt::AlignCenter);
    d->errorView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->errorView->setLineWidth(1);

    QVBoxLayout* const vbox1 = new QVBoxLayout(d->errorView);
    vbox1->addWidget(errorMsg, 10);
    vbox1->setContentsMargins(margins);
    vbox1->setSpacing(spacing);

    insertWidget(Private::ErrorView, d->errorView);

    // --------------------------------------------------------------------------

    d->playerView  = new QFrame(this);
    d->videoWidget = new QVideoWidget(this);
    d->player      = new QMediaPlayer(this);

    d->slider      = new QSlider(Qt::Horizontal, this);
    d->slider->setRange(0, 0);

    d->player->setVideoOutput(d->videoWidget);

    d->videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    d->videoWidget->setStyleSheet(QLatin1String("background-color:black;"));

    d->playerView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->playerView->setLineWidth(1);

    QVBoxLayout* const vbox2 = new QVBoxLayout(d->playerView);
    vbox2->addWidget(d->videoWidget, 10);
    vbox2->addWidget(d->slider,       0);
    vbox2->setContentsMargins(margins);
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

    // --------------------------------------------------------------------------

    connect(this, SIGNAL(signalFinished()),
            this, SLOT(slotPlayerFinished()));

    connect(d->player, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)),
            this, SLOT(slotPlayerStateChanged(QMediaPlayer::PlaybackState)));

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

    connect(d->slider, SIGNAL(sliderPressed()),
            this, SLOT(slotSliderPressed()));

    connect(d->slider, SIGNAL(sliderReleased()),
            this, SLOT(slotSliderReleased()));

    connect(d->slider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotPosition(int)));

    connect(d->player, SIGNAL(positionChanged(qint64)),
            this, SLOT(slotPositionChanged(qint64)));

    connect(d->player, SIGNAL(durationChanged(qint64)),
            this, SLOT(slotDurationChanged(qint64)));

    connect(d->player, SIGNAL(errorOccurred(QMediaPlayer::Error,QString)),
            this, SLOT(slotHandlePlayerError(QMediaPlayer::Error,QString)));

    qCDebug(DIGIKAM_GENERAL_LOG) << "Using QtMultimedia";
}

MediaPlayerView::~MediaPlayerView()
{
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
    d->player->play();
}

void MediaPlayerView::slotPlayerFinished()
{
    if (
        (d->player->playbackState() == QMediaPlayer::StoppedState)  &&
        (d->player->error() != QMediaPlayer::NoError)
       )
    {
        setPreviewMode(Private::ErrorView);
    }
}

void MediaPlayerView::slotPlayerStateChanged(QMediaPlayer::PlaybackState newState)
{
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
        Q_EMIT signalFinished();
    }
}

void MediaPlayerView::escapePreview()
{
    d->player->stop();
}

void MediaPlayerView::slotRotateVideo()
{
    // TODO
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

    if (url.isEmpty())
    {
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

    d->player->setSource(d->currentItem);
    setPreviewMode(Private::PlayerView);
    d->player->play();
}

void MediaPlayerView::slotPositionChanged(qint64 position)
{
    if (!d->slider->isSliderDown())
    {
        d->slider->setValue(position);
    }
}

void MediaPlayerView::slotDurationChanged(qint64 duration)
{
    d->slider->setRange(0, duration);
}

void MediaPlayerView::slotPosition(int position)
{
    if (d->player->isSeekable())
    {
        d->player->setPosition((qint64)position);
    }
}

void MediaPlayerView::slotSliderPressed()
{
    if (
        (d->player->playbackState() == QMediaPlayer::PlayingState) ||
        (d->player->mediaStatus()   == QMediaPlayer::EndOfMedia)
       )
    {
        d->player->pause();
    }
}

void MediaPlayerView::slotSliderReleased()
{
    if (d->player->mediaStatus() != QMediaPlayer::EndOfMedia)
    {
        d->player->play();
    }
}

void MediaPlayerView::slotHandlePlayerError(QMediaPlayer::Error /*error*/, const QString& errStr)
{
    setPreviewMode(Private::ErrorView);

    qCDebug(DIGIKAM_GENERAL_LOG) << "Error: " << errStr;
}

}  // namespace Digikam

#include "mediaplayerview.moc"
