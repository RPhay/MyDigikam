/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "MainWindow_p.h"

namespace AVPlayer
{

void MainWindow::onSeekFinished(qint64 pos)
{
    qCDebug(DIGIKAM_AVPLAYER_LOG) << "seek finished at"
                                  << pos
                                  << "/"
                                  << d->pPlayer->position();
}

void MainWindow::stopUnload()
{
    d->pPlayer->stop();
}

void MainWindow::processPendingActions()
{
    if (d->HasPendingPlay)
    {
        d->HasPendingPlay = false;
        play(d->File);
    }
}


void MainWindow::play(const QString& name)
{
    d->File = name;

    if (!d->IsReady)
    {
        d->HasPendingPlay = true;

        return;
    }

    d->Title = d->File;

    if (!d->File.contains(QLatin1String("://")) || d->File.startsWith(QLatin1String("file://")))
    {
        d->Title = QFileInfo(d->File).fileName();
    }

    setWindowTitle(d->Title);
    d->pPlayer->stop();       // if no stop, in setPriority decoder will reopen
    d->pPlayer->setFrameRate(AVPlayerConfigMngr::instance().forceFrameRate());

    if (!d->AudioBackends.isEmpty())
        d->pPlayer->audio()->setBackends(d->AudioBackends);

    if (!d->pRepeatEnableAction->isChecked())
        d->RepeateMax = 0;

    d->pPlayer->setInterruptOnTimeout(AVPlayerConfigMngr::instance().abortOnTimeout());
    d->pPlayer->setInterruptTimeout(AVPlayerConfigMngr::instance().timeout() * 1000.0);
    d->pPlayer->setBufferMode(QtAV::BufferPackets);
    d->pPlayer->setBufferValue(AVPlayerConfigMngr::instance().bufferValue());
    d->pPlayer->setRepeat(d->RepeateMax);
    d->pPlayer->setPriority(DecoderConfigPage::idsFromNames(AVPlayerConfigMngr::instance().decoderPriorityNames()));
    d->pPlayer->setOptionsForAudioCodec(d->pDecoderConfigPage->audioDecoderOptions());
    d->pPlayer->setOptionsForVideoCodec(d->pDecoderConfigPage->videoDecoderOptions());

    if (AVPlayerConfigMngr::instance().avformatOptionsEnabled())
        d->pPlayer->setOptionsForFormat(AVPlayerConfigMngr::instance().avformatOptions());

    qCDebug(DIGIKAM_AVPLAYER_LOG) << AVPlayerConfigMngr::instance().avformatOptions();

    PlayListItem item;
    item.setUrl(d->File);
    item.setTitle(d->Title);
    item.setLastTime(0);
    d->pHistory->remove(d->File);
    d->pHistory->insertItemAt(item, 0);
    d->pPlayer->play(name);
}

void MainWindow::play(const QUrl& url)
{
    play(QUrl::fromPercentEncoding(url.toEncoded()));
}

void MainWindow::openFile()
{
    QString file = QFileDialog::getOpenFileName(nullptr, i18nc("@title:window", "Open a Media File"),
                                                AVPlayerConfigMngr::instance().lastFile());

    if (file.isEmpty())
        return;

    AVPlayerConfigMngr::instance().setLastFile(file);
    play(file);
}

void MainWindow::togglePlayPause()
{
    if (d->pPlayer->isPlaying())
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("isPaused = %d", d->pPlayer->isPaused());
        d->pPlayer->pause(!d->pPlayer->isPaused());
    }
    else
    {
        if (d->File.isEmpty())
            return;

        if (!d->pPlayer->isPlaying())
            play(d->File);
        else
            d->pPlayer->play();

        d->pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("player-volume")));
    }
}

void MainWindow::showNextOSD()
{
    if (!d->pOSD)
        return;

    d->pOSD->useNextShowType();
}

void MainWindow::onSpinBoxChanged(double v)
{
    if (!d->pPlayer)
        return;

    d->pPlayer->setSpeed(v);
}

void MainWindow::onPaused(bool p)
{
    if (p)
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("start pausing...");
        d->pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    }
    else
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("stop pausing...");
        d->pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
    }
}

void MainWindow::onStartPlay()
{
    d->pRenderer->setRegionOfInterest(QRectF());
    d->File  = d->pPlayer->file();       // open from EventFilter's menu
    d->Title = d->File;

    if (!d->File.contains(QLatin1String("://")) || d->File.startsWith(QLatin1String("file://")))
        d->Title = QFileInfo(d->File).fileName();

    setWindowTitle(d->Title);

    d->pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
    d->pTimeSlider->setMinimum(d->pPlayer->mediaStartPosition());
    d->pTimeSlider->setMaximum(d->pPlayer->mediaStopPosition());
    d->pTimeSlider->setValue(0);
    d->pTimeSlider->setEnabled(d->pPlayer->isSeekable());
    d->pEnd->setText(QTime(0, 0, 0).addMSecs(d->pPlayer->mediaStopPosition()).toString(QString::fromLatin1("HH:mm:ss")));
    setVolume();
    d->ShowControl = 0;

    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));

    ScreenSaver::instance().disable();
    initAudioTrackMenu();
    d->pRepeatA->setMinimumTime(QTime(0, 0, 0).addMSecs(d->pPlayer->mediaStartPosition()));
    d->pRepeatA->setMaximumTime(QTime(0, 0, 0).addMSecs(d->pPlayer->mediaStopPosition()));
    d->pRepeatB->setMinimumTime(QTime(0, 0, 0).addMSecs(d->pPlayer->mediaStartPosition()));
    d->pRepeatB->setMaximumTime(QTime(0, 0, 0).addMSecs(d->pPlayer->mediaStopPosition()));
    d->pRepeatA->setTime(QTime(0, 0, 0).addMSecs(d->pPlayer->startPosition()));
    d->pRepeatB->setTime(QTime(0, 0, 0).addMSecs(d->pPlayer->stopPosition()));
    d->CursorTimer      = startTimer(3000);
    PlayListItem item = d->pHistory->itemAt(0);
    item.setUrl(d->File);
    item.setTitle(d->Title);
    item.setDuration(d->pPlayer->duration());
    d->pHistory->setItemAt(item, 0);
    updateChannelMenu();

    if (d->pStatisticsView && d->pStatisticsView->isVisible())
        d->pStatisticsView->setStatistics(d->pPlayer->statistics());
}

void MainWindow::onStopPlay()
{
    d->pPlayer->setPriority(DecoderConfigPage::idsFromNames(AVPlayerConfigMngr::instance().decoderPriorityNames()));

    if ((d->pPlayer->currentRepeat() >= 0) && (d->pPlayer->currentRepeat() < d->pPlayer->repeat()))
        return;

    // use shortcut to replay in EventFilter, the options will not be set, so set here

    d->pPlayer->setFrameRate(AVPlayerConfigMngr::instance().forceFrameRate());
    d->pPlayer->setOptionsForAudioCodec(d->pDecoderConfigPage->audioDecoderOptions());
    d->pPlayer->setOptionsForVideoCodec(d->pDecoderConfigPage->videoDecoderOptions());

    if (AVPlayerConfigMngr::instance().avformatOptionsEnabled())
        d->pPlayer->setOptionsForFormat(AVPlayerConfigMngr::instance().avformatOptions());

    d->pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    d->pTimeSlider->setValue(0);

    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("disable slider");

    d->pTimeSlider->setDisabled(true);
    d->pTimeSlider->setMinimum(0);
    d->pTimeSlider->setMaximum(0);
    d->pCurrent->setText(QString::fromLatin1("00:00:00"));
    d->pEnd->setText(QString::fromLatin1("00:00:00"));
    tryShowControlBar();
    ScreenSaver::instance().enable();
    toggleRepeat(false);
/*
    d->RepeateMax = 0;
*/
    killTimer(d->CursorTimer);
    unsetCursor();

    if (d->preview)
        d->preview->setFile(QString());
}

void MainWindow::onSpeedChange(qreal speed)
{
    d->pSpeed->setText(QString::fromLatin1("%1").arg(speed, 4, 'f', 2, QLatin1Char('0')));
}

void MainWindow::seek(int value)
{
    d->pPlayer->setSeekType(AccurateSeek);
    d->pPlayer->seek((qint64)value);

    if (!d->preview || !AVPlayerConfigMngr::instance().previewEnabled())
        return;

    d->preview->setTimestamp(value);
    d->preview->preview();
    d->preview->setWindowFlags(d->preview->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    d->preview->resize(AVPlayerConfigMngr::instance().previewWidth(), AVPlayerConfigMngr::instance().previewHeight());
    d->preview->show();
}

void MainWindow::seek()
{
    seek(d->pTimeSlider->value());
}

void MainWindow::showHideVolumeBar()
{
    if (d->pVolumeSlider->isHidden())
    {
        d->pVolumeSlider->show();
    }
    else
    {
        d->pVolumeSlider->hide();
    }
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    Q_UNUSED(e);

    if (d->pPlayer)
        d->pPlayer->stop();

    qApp->quit();
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    QWidget::resizeEvent(e);
/*
    if (d->pTitle)
        d->QLabelSetElideText(d->pTitle, QFileInfo(d->File).fileName(), e->size().width());
*/
}

void MainWindow::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == d->CursorTimer)
    {
        if (d->pControl->isVisible())
            return;

        setCursor(Qt::BlankCursor);
    }
}

void MainWindow::onPositionChange(qint64 pos)
{
    if (d->pPlayer->isSeekable())
        d->pTimeSlider->setValue(pos);

    d->pCurrent->setText(QTime(0, 0, 0).addMSecs(pos).toString(QString::fromLatin1("HH:mm:ss")));
/*
    setWindowTitle(QString::number(d->pPlayer->statistics().video_only.currentDisplayFPS(),
                                   'f', 2).append(" ").append(d->Title));
*/
}

void MainWindow::repeatAChanged(const QTime& t)
{
    if (!d->pPlayer)
        return;

    d->pPlayer->setStartPosition(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::repeatBChanged(const QTime& t)
{
    if (!d->pPlayer)
        return;

    // when this slot is called? even if only range is set?

    if (t <= d->pRepeatA->time())
        return;

    d->pPlayer->setStopPosition(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    d->ControlOn = (e->key() == Qt::Key_Control);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e)
{
    Q_UNUSED(e);
    d->ControlOn = false;
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    if (!d->ControlOn)
        return;

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))

    d->GlobalMouse = e->globalPosition();

#else

    d->GlobalMouse = e->globalPos();

#endif

}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
    unsetCursor();

    if (e->pos().y() > (height() - d->pTimeSlider->height() - d->pControl->height()))
    {
        if (d->ShowControl == 0)
        {
            d->ShowControl = 1;
            tryShowControlBar();
        }
    }
    else
    {
        if (d->ShowControl == 1)
        {
            d->ShowControl = 0;
            QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
        }
    }

    if (d->ControlOn && (e->button() == Qt::LeftButton))
    {
        if (!d->pRenderer || !d->pRenderer->widget())
            return;

        QRectF roi    = d->pRenderer->realROI();
        QPointF delta = e->pos() - d->GlobalMouse;
        roi.moveLeft(-delta.x());
        roi.moveTop(-delta.y());
        d->pRenderer->setRegionOfInterest(roi);
    }
}

void MainWindow::wheelEvent(QWheelEvent* e)
{
    if (!d->pRenderer || !d->pRenderer->widget())
    {
        return;
    }

    QPoint dp;

    qreal deg = e->angleDelta().y() / 8.0;
    dp        = e->pixelDelta();

#ifdef WHEEL_SPEED

    if (!d->ControlOn)
    {
        qreal speed = d->pPlayer->speed();
        d->pPlayer->setSpeed(qMax(0.01, speed + deg / 15.0*0.02));

        return;
    }

#endif // WHEEL_SPEED

    QPointF p  = d->pRenderer->widget()->mapFrom(this, e->position().toPoint());
    QPointF fp = d->pRenderer->mapToFrame(p);
/*
    qCDebug(DIGIKAM_AVPLAYER_LOG) <<  p << fp;
*/
    if (fp.x() < 0)
    {
        fp.setX(0);
    }

    if (fp.y() < 0)
    {
        fp.setY(0);
    }

    if (fp.x() > d->pRenderer->videoFrameSize().width())
    {
        fp.setX(d->pRenderer->videoFrameSize().width());
    }

    if (fp.y() > d->pRenderer->videoFrameSize().height())
    {
        fp.setY(d->pRenderer->videoFrameSize().height());
    }

    QRectF viewport = QRectF(d->pRenderer->mapToFrame(QPointF(0, 0)),
                             d->pRenderer->mapToFrame(QPointF(d->pRenderer->rendererWidth(),
                             d->pRenderer->rendererHeight())));

    Q_UNUSED(viewport);
/*
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
        << QString::asprintf("vo: (%.1f, %.1f)=> frame: (%.1f, %.1f)",
            p.x(), p.y(), fp.x(), fp.y());
*/
    qreal zoom = 1.0 + deg * M_PI / 180.0;

    if (!dp.isNull())
    {
        zoom = 1.0 + (qreal)dp.y() / 100.0;
    }

    static qreal z = 1.0;
    z             *= zoom;

    if (z < 1.0)
    {
        z = 1.0;
    }

    qreal x0 = fp.x() - fp.x() / z;
    qreal y0 = fp.y() - fp.y() / z;
/*
    qCDebug(DIGIKAM_AVPLAYER_LOG) << "fr: "
                                  << QRectF(x0, y0, qreal(d->pRenderer->videoFrameSize().width()) / z,
                                                          qreal(d->pRenderer->videoFrameSize().height()) / z)
                                  << fp << z;
*/
    d->pRenderer->setRegionOfInterest(QRectF(x0, y0,
                                           qreal(d->pRenderer->videoFrameSize().width())  / z,
                                           qreal(d->pRenderer->videoFrameSize().height()) / z));

/*
    QTransform m;
    m.translate(fp.x(), fp.y());
    m.scale(1.0/zoom, 1.0/zoom);
    m.translate(-fp.x(), -fp.y());
    QRectF r = m.mapRect(d->pRenderer->realROI());
    d->pRenderer->setRegionOfInterest((r | m.mapRect(viewport))&QRectF(QPointF(0,0), d->pRenderer->videoFrameSize()));
*/
}

void MainWindow::about()
{
    QtAV::about();
}

void MainWindow::openUrl()
{
    QString url = QInputDialog::getText(nullptr, i18nc("@title:window", "Open an Url"),
                                        i18nc("@info", "Url"));

    if (url.isEmpty())
        return;

    play(url);
}

void MainWindow::updateChannelMenu()
{
    if (d->pChannelAction)
        d->pChannelAction->setChecked(false);

    AudioOutput* const ao = (d->pPlayer ? d->pPlayer->audio() : nullptr); // getAO() ?

    if (!ao)
    {
        return;
    }

    AudioFormat::ChannelLayout cl = ao->audioFormat().channelLayout();
    QList<QAction*> as            = d->pChannelMenu->actions();

    Q_FOREACH (QAction* const action, as)
    {
        if (action->data().toInt() != (int)cl)
            continue;

        action->setChecked(true);
        d->pChannelAction = action;

        break;
    }
}

void MainWindow::switchAspectRatio(QAction *action)
{
    qreal r = action->data().toDouble();

    if ((action == d->pARAction) && (r != -2))
    {
        action->toggle(); // check state changes if clicked

        return;
    }

    if      (r == 0)
    {
        d->pPlayer->renderer()->setOutAspectRatioMode(QtAV::VideoAspectRatio);
    }
    else if (r == -1)
    {
        d->pPlayer->renderer()->setOutAspectRatioMode(QtAV::RendererAspectRatio);
    }
    else
    {
        if (r == -2)
            r = QInputDialog::getDouble(0, i18nc("@option", "Aspect ratio"), QString(), 1.0);

        d->pPlayer->renderer()->setOutAspectRatioMode(QtAV::CustomAspectRation);
        d->pPlayer->renderer()->setOutAspectRatio(r);
    }

    d->pARAction->setChecked(false);
    d->pARAction = action;
    d->pARAction->setChecked(true);
}

void MainWindow::toggleRepeat(bool r)
{
    d->pRepeatEnableAction->setChecked(r);
    d->pRepeatAction->defaultWidget()->setEnabled(r); // why need defaultWidget?

    if (r)
    {
        d->RepeateMax = d->pRepeatBox->value();
    }
    else
    {
        d->RepeateMax = 0;
    }

    if (d->pPlayer)
    {
        d->pPlayer->setRepeat(d->RepeateMax);

        if (r)
        {
            repeatAChanged(d->pRepeatA->time());
            repeatBChanged(d->pRepeatB->time());
        }
        else
        {
            d->pPlayer->setTimeRange(0);
        }
    }
}

void MainWindow::playOnlineVideo(QAction* action)
{
    d->Title = action->text();
    play(action->data().toString());
}

void MainWindow::onPlayListClick(const QString& key, const QString& value)
{
    d->Title = key;
    play(value);
}

void MainWindow::tryHideControlBar()
{
    if (d->ShowControl > 0)
    {
        return;
    }

    if (d->pControl->isHidden() && d->pTimeSlider->isHidden())
        return;

    d->pControl->hide();
    d->pTimeSlider->hide();
    d->workaroundRendererSize(this);
}

void MainWindow::tryShowControlBar()
{
    unsetCursor();

    if (d->pTimeSlider && d->pTimeSlider->isHidden())
        d->pTimeSlider->show();

    if (d->pControl && d->pControl->isHidden())
        d->pControl->show();
}

void MainWindow::showInfo()
{
    if (!d->pStatisticsView)
        d->pStatisticsView = new StatisticsView();

    if (d->pPlayer)
        d->pStatisticsView->setStatistics(d->pPlayer->statistics());

    d->pStatisticsView->show();
}

void MainWindow::onTimeSliderHover(int pos, int value)
{
    QPoint gpos = mapToGlobal(d->pTimeSlider->pos() + QPoint(pos, 0));
    QToolTip::showText(gpos, QTime(0, 0, 0).addMSecs(value).toString(QString::fromLatin1("HH:mm:ss")));

    if (!AVPlayerConfigMngr::instance().previewEnabled())
        return;

    if (!d->preview)
        d->preview = new VideoPreviewWidget();

    d->preview->setFile(d->pPlayer->file());
    d->preview->setTimestamp(value);
    d->preview->preview();
    const int w = AVPlayerConfigMngr::instance().previewWidth();
    const int h = AVPlayerConfigMngr::instance().previewHeight();
    d->preview->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    d->preview->resize(w, h);
    d->preview->move(gpos - QPoint(w / 2, h));
    d->preview->show();
}

void MainWindow::onTimeSliderLeave()
{
/*
    if (d->preview && d->preview->isVisible())
        d->preview->hide();
*/
    if (!d->preview)
    {
        return;
    }

    if (d->preview->isVisible())
    {
        d->preview->close();
    }

    delete d->preview;
    d->preview = nullptr;
}

void MainWindow::handleError(const AVError& e)
{
    QMessageBox::warning(nullptr, i18nc("@title:window", "Player Error"), e.string());
}

void MainWindow::onMediaStatusChanged()
{
    QString status;
    QtAV::AVPlayerCore* const player = reinterpret_cast<QtAV::AVPlayerCore*>(sender());

    if (!player)
    {
        // why it happens? reinterpret_cast  works.

        qCWarning(DIGIKAM_AVPLAYER_LOG) << "invalid sender() "
                                        << sender() << player;

        return;
    }

    switch (player->mediaStatus())
    {
        case NoMedia:
        {
            status = i18nc("@info: media loading", "No media");

            break;
        }

        case InvalidMedia:
        {
            status = i18nc("@info: media loading", "Invalid media");

            break;
        }

        case BufferingMedia:
        {
            status = i18nc("@info: media loading", "Buffering...");

            break;
        }

        case BufferedMedia:
        {
            status = i18nc("@info: media loading", "Buffered");

            break;
        }

        case LoadingMedia:
        {
            status = i18nc("@info: media loading", "Loading...");

            break;
        }

        case LoadedMedia:
        {
            status = i18nc("@info: media loading", "Loaded");

            break;
        }

        case StalledMedia:
        {
            status = i18nc("@info: media loading", "Stalled");

            break;
        }

        default:
        {
            status = QString();
            onStopPlay();

            break;
        }
    }

    qCDebug(DIGIKAM_AVPLAYER_LOG) << "status changed " << status;
    setWindowTitle(status + QString::fromLatin1(" ") + d->Title);
}

void MainWindow::onBufferProgress(qreal percent)
{
    const qreal bs = d->pPlayer->bufferSpeed();
    QString s;

    if      (bs > 1024 * 1024 * 1024)
        s = QString::fromLatin1("%1G/s").arg(bs / 1024.0 / 1024.0 / 1024.0, 6, 'f', 1);
    else if (bs > 1024*1024)
        s = QString::fromLatin1("%1M/s").arg(bs / 1024.0 / 1024.0, 6, 'f', 1);
    else if (bs > 1024)
        s = QString::fromLatin1("%1K/s").arg(bs / 1024.0, 6, 'f', 1);
    else
        s = QString::fromLatin1("%1B/s").arg(bs, 6, 'f', 1);

    setWindowTitle(i18nc("@title:window, media loading state", "Buffering... %1% @%2 %3",
                   QString::fromLatin1("%1").arg(percent*100.0, 5, 'f', 1), s, d->Title));
}

void MainWindow::onVideoEQEngineChanged()
{
    VideoRenderer* const vo     = d->pPlayer->renderer();
    VideoEQConfigPage::Engine e = d->pVideoEQ->engine();

    if (
        (e == VideoEQConfigPage::SWScale) &&
        (vo->id() != VideoRendererId_X11) // X11 scales in the renderer
       )
    {
        vo->forcePreferredPixelFormat(true);
        vo->setPreferredPixelFormat(VideoFormat::Format_RGB32);
    }
    else
    {
        vo->forcePreferredPixelFormat(false);
    }

    onBrightnessChanged(d->pVideoEQ->brightness() * 100.0);
    onContrastChanged(d->pVideoEQ->contrast() * 100.0);
    onHueChanged(d->pVideoEQ->hue() * 100.0);
    onSaturationChanged(d->pVideoEQ->saturation() * 100.0);
}

void MainWindow::onBrightnessChanged(int b)
{
    VideoRenderer* const vo = d->pPlayer->renderer();

    if ((d->pVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setBrightness(d->pVideoEQ->brightness()))
    {
        d->pPlayer->setBrightness(0);
    }
    else
    {
        vo->setBrightness(0);
        d->pPlayer->setBrightness(b);
    }
}

void MainWindow::onContrastChanged(int c)
{
    VideoRenderer* const vo = d->pPlayer->renderer();

    if ((d->pVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setContrast(d->pVideoEQ->contrast()))
    {
        d->pPlayer->setContrast(0);
    }
    else
    {
        vo->setContrast(0);
        d->pPlayer->setContrast(c);
    }
}

void MainWindow::onHueChanged(int h)
{
    VideoRenderer* const vo = d->pPlayer->renderer();

    if ((d->pVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setHue(d->pVideoEQ->hue()))
    {
        d->pPlayer->setHue(0);
    }
    else
    {
        vo->setHue(0);
        d->pPlayer->setHue(h);
    }
}

void MainWindow::onSaturationChanged(int s)
{
    VideoRenderer* const vo = d->pPlayer->renderer();

    if ((d->pVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setSaturation(d->pVideoEQ->saturation()))
    {
        d->pPlayer->setSaturation(0);
    }
    else
    {
        vo->setSaturation(0);
        d->pPlayer->setSaturation(s);
    }
}

void MainWindow::onCaptureConfigChanged()
{
    d->pPlayer->videoCapture()->setCaptureDir(AVPlayerConfigMngr::instance().captureDir());
    d->pPlayer->videoCapture()->setQuality(AVPlayerConfigMngr::instance().captureQuality());

    if (AVPlayerConfigMngr::instance().captureFormat().toLower() == QLatin1String("original"))
    {
        d->pPlayer->videoCapture()->setOriginalFormat(true);
    }
    else
    {
        d->pPlayer->videoCapture()->setOriginalFormat(false);
        d->pPlayer->videoCapture()->setSaveFormat(AVPlayerConfigMngr::instance().captureFormat());
    }

    d->pCaptureBtn->setToolTip(i18nc("@info", "Capture video frame\nSave to: %1\nFormat: %2",
                             d->pPlayer->videoCapture()->captureDir(),
                             AVPlayerConfigMngr::instance().captureFormat()));
}

void MainWindow::onAVFilterVideoConfigChanged()
{
    if (d->pVideoFilter)
    {
        d->pVideoFilter->uninstall();
        delete d->pVideoFilter;
        d->pVideoFilter = nullptr;
    }

    d->pVideoFilter = new LibAVFilterVideo(this);
    d->pVideoFilter->setEnabled(AVPlayerConfigMngr::instance().avfilterVideoEnable());
    d->pPlayer->installFilter(d->pVideoFilter);
    d->pVideoFilter->setOptions(AVPlayerConfigMngr::instance().avfilterVideoOptions());
}

void MainWindow::onAVFilterAudioConfigChanged()
{
    if (d->pAudioFilter)
    {
        d->pAudioFilter->uninstall();
        delete d->pAudioFilter;
        d->pAudioFilter = nullptr;
    }

    d->pAudioFilter = new LibAVFilterAudio(this);
    d->pAudioFilter->setEnabled(AVPlayerConfigMngr::instance().avfilterAudioEnable());
    d->pAudioFilter->installTo(d->pPlayer);
    d->pAudioFilter->setOptions(AVPlayerConfigMngr::instance().avfilterAudioOptions());
}

void MainWindow::onBufferValueChanged()
{
    if (!d->pPlayer)
        return;

    d->pPlayer->setBufferValue(AVPlayerConfigMngr::instance().bufferValue());
}

void MainWindow::onAbortOnTimeoutChanged()
{
    if (!d->pPlayer)
        return;

    d->pPlayer->setInterruptOnTimeout(AVPlayerConfigMngr::instance().abortOnTimeout());
}

void MainWindow::onUserShaderChanged()
{
    if (!d->pRenderer || !d->pRenderer->opengl())
        return;

#ifndef QT_NO_OPENGL

    if (AVPlayerConfigMngr::instance().userShaderEnabled())
    {
        if (AVPlayerConfigMngr::instance().intermediateFBO())
        {
            if (!d->glsl)
                d->glsl = new GLSLFilter(this);

            d->glsl->installTo(d->pRenderer);
        }
        else
        {
            if (d->glsl)
                d->glsl->uninstall();
        }

        if (!d->shader)
            d->shader = new DynamicShaderObject(this);

        d->shader->setHeader(AVPlayerConfigMngr::instance().fragHeader());
        d->shader->setSample(AVPlayerConfigMngr::instance().fragSample());
        d->shader->setPostProcess(AVPlayerConfigMngr::instance().fragPostProcess());
        d->pRenderer->opengl()->setUserShader(d->shader);
    }
    else
    {
        d->pRenderer->opengl()->setUserShader(nullptr);
    }

#endif

}

void MainWindow::handleFullscreenChange()
{
    d->workaroundRendererSize(this);

    // workaround renderer display size for ubuntu

    tryShowControlBar();

    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
}

void MainWindow::toggoleSubtitleEnabled(bool value)
{
    d->pSubtitle->setEnabled(value);
}

void MainWindow::toggleSubtitleAutoLoad(bool value)
{
    d->pSubtitle->setAutoLoad(value);
}

void MainWindow::openSubtitle()
{
    QString file = QFileDialog::getOpenFileName(nullptr, i18nc("@title:window", "Open a Subtitle File"));

    if (file.isEmpty())
        return;

    d->pSubtitle->setFile(file);
}

void MainWindow::setSubtitleCharset(int)
{
    QComboBox* const box = qobject_cast<QComboBox*>(sender());

    if (!box)
        return;

    d->pSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
}

void MainWindow::setSubtitleEngine(int)
{
    QComboBox* const box = qobject_cast<QComboBox*>(sender());

    if (!box)
        return;

    d->pSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
}

void MainWindow::syncVolumeUi(qreal value)
{
    const int v(value / d->kVolumeInterval);

    if (d->pVolumeSlider->value() == v)
        return;

    d->pVolumeSlider->setValue(v);
}

} // namespace AVPlayer
