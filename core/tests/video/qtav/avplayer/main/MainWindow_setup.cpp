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

void MainWindow::initPlayer()
{
    d->pPlayer                = new QtAV::AVPlayerCore(this);
    d->IsReady                = true;
    VideoRenderer* const vo = VideoRenderer::create((VideoRendererId)property("rendererId").toInt());

    if (!vo || !vo->isAvailable() || !vo->widget())
    {
        QMessageBox::critical(nullptr, QString::fromLatin1("AVPlayer"),
                              i18nc("@info", "Video renderer is not available on your platform!"));
    }

    setRenderer(vo);
/*
    d->pSubtitle->installTo(d->pPlayer); //filter on frame
*/
    d->pSubtitle->setPlayer(d->pPlayer);
/*
    d->pPlayer->setAudioOutput(AudioOutputFactory::create(AudioOutputId_OpenAL));
*/
    EventFilter* const ef = new EventFilter(d->pPlayer);
    qApp->installEventFilter(ef);

    connect(ef, SIGNAL(showNextOSD()),
            this, SLOT(showNextOSD()));

    onCaptureConfigChanged();
    onAVFilterVideoConfigChanged();
    onAVFilterAudioConfigChanged();

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(forceFrameRateChanged()),
            this, SLOT(setFrameRate()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(captureDirChanged(QString)),
            this, SLOT(onCaptureConfigChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(captureFormatChanged(QString)),
            this, SLOT(onCaptureConfigChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(captureQualityChanged(int)),
            this, SLOT(onCaptureConfigChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(avfilterVideoChanged()),
            this, SLOT(onAVFilterVideoConfigChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(avfilterAudioChanged()),
            this, SLOT(onAVFilterAudioConfigChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(bufferValueChanged()),
            this, SLOT(onBufferValueChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(abortOnTimeoutChanged()),
            this, SLOT(onAbortOnTimeoutChanged()));

    connect(d->pStopBtn, SIGNAL(clicked()),
            this, SLOT(stopUnload()));

    connect(d->pForwardBtn, SIGNAL(clicked()),
             d->pPlayer, SLOT(seekForward()));

    connect(d->pBackwardBtn, SIGNAL(clicked()),
            d->pPlayer, SLOT(seekBackward()));

    connect(d->pVolumeBtn, SIGNAL(clicked()),
            this, SLOT(showHideVolumeBar()));

    connect(d->pVolumeSlider, SIGNAL(sliderPressed()),
            this, SLOT(setVolume()));

    connect(d->pVolumeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setVolume()));

    connect(d->pPlayer, SIGNAL(seekFinished(qint64)),
            this, SLOT(onSeekFinished(qint64)));

    connect(d->pPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),
            this, SLOT(onMediaStatusChanged()));

    connect(d->pPlayer, SIGNAL(bufferProgressChanged(qreal)),
            this, SLOT(onBufferProgress(qreal)));

    connect(d->pPlayer, SIGNAL(error(QtAV::AVError)),
            this, SLOT(handleError(QtAV::AVError)));

    connect(d->pPlayer, SIGNAL(started()),
            this, SLOT(onStartPlay()));

    connect(d->pPlayer, SIGNAL(stopped()),
            this, SLOT(onStopPlay()));

    connect(d->pPlayer, SIGNAL(paused(bool)),
            this, SLOT(onPaused(bool)));

    connect(d->pPlayer, SIGNAL(speedChanged(qreal)),
            this, SLOT(onSpeedChange(qreal)));

    connect(d->pPlayer, SIGNAL(positionChanged(qint64)),
            this, SLOT(onPositionChange(qint64)));
/*
    connect(d->pPlayer, SIGNAL(volumeChanged(qreal)),
            this, SLOT(syncVolumeUi(qreal)));
*/
    connect(d->pVideoEQ, SIGNAL(brightnessChanged(int)),
            this, SLOT(onBrightnessChanged(int)));

    connect(d->pVideoEQ, SIGNAL(contrastChanged(int)),
            this, SLOT(onContrastChanged(int)));

    connect(d->pVideoEQ, SIGNAL(hueChanegd(int)),
            this, SLOT(onHueChanged(int)));

    connect(d->pVideoEQ, SIGNAL(saturationChanged(int)),
            this, SLOT(onSaturationChanged(int)));

    connect(d->pCaptureBtn, SIGNAL(clicked()),
            d->pPlayer->videoCapture(), SLOT(capture()));

    // Emit this signal after connection.
    // otherwise the Q_SLOTS may not be called for the first time

    Q_EMIT ready();
}

void MainWindow::initAudioTrackMenu()
{
    int track  = -2;
    QAction* a = nullptr;
    QList<QAction*> as;
    int tracks = 0;

    if (!d->pPlayer)
    {
        a = d->pAudioTrackMenu->addAction(i18nc("@action: audio track", "External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        d->pAudioTrackAction = 0;

        goto end;
    }

    track  = d->pPlayer->currentAudioStream();
    as     = d->pAudioTrackMenu->actions();
    tracks = d->pPlayer->audioStreamCount();

    if (d->pAudioTrackAction && ((tracks == as.size() - 1)) && (d->pAudioTrackAction->data().toInt() == track))
        return;

    while ((tracks + 1) < as.size())
    {
        a = as.takeLast();
        d->pAudioTrackMenu->removeAction(a);
        delete a;
    }

    if (as.isEmpty())
    {
        a = d->pAudioTrackMenu->addAction(i18nc("@action: audio track", "External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        d->pAudioTrackAction = 0;
    }

    while ((tracks + 1) > as.size())
    {
        a = d->pAudioTrackMenu->addAction(QString::number(as.size()-1));
        a->setData(as.size()-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
    }

end:

    Q_FOREACH (QAction* const ac, as)
    {
        if ((ac->data().toInt() == track) && (track >= 0))
        {
            if (d->pPlayer && d->pPlayer->externalAudio().isEmpty())
            {
                qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                    << QString::asprintf("track found!");

                d->pAudioTrackAction = ac;
                ac->setChecked(true);
            }
        }
        else
        {
            ac->setChecked(false);
        }
    }

    if (d->pPlayer && !d->pPlayer->externalAudio().isEmpty())
    {
        d->pAudioTrackAction = as.first();
    }

    if (d->pAudioTrackAction)
        d->pAudioTrackAction->setChecked(true);
}

void MainWindow::setupUi()
{
    QVBoxLayout* const mainLayout   = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(QMargins());
    setLayout(mainLayout);

    d->pPlayerLayout                  = new QVBoxLayout();
    d->pControl                       = new QWidget(this);
    d->pControl->setMaximumHeight(30);
/*
    d->pPreview = new QLabel(this);
*/
    d->pTimeSlider                    = new AVPlayerSlider(d->pControl);
    d->pTimeSlider->setDisabled(true);
    d->pTimeSlider->setTracking(true);
    d->pTimeSlider->setOrientation(Qt::Horizontal);
    d->pTimeSlider->setMinimum(0);
    d->pCurrent                       = new QLabel(d->pControl);
    d->pCurrent->setToolTip(i18nc("@info", "Current time"));
    d->pCurrent->setContentsMargins(QMargins(2, 2, 2, 2));
    d->pCurrent->setText(QString::fromLatin1("00:00:00"));
    d->pEnd                           = new QLabel(d->pControl);
    d->pEnd->setToolTip(i18nc("@info", "Duration"));
    d->pEnd->setContentsMargins(QMargins(2, 2, 2, 2));
    d->pEnd->setText(QString::fromLatin1("00:00:00"));
    d->pTitle                         = new QLabel(d->pControl);
    d->pTitle->setToolTip(i18nc("@info", "Render engine"));
    d->pTitle->setText(QString::fromLatin1("QPainter"));
    d->pTitle->setIndent(8);
    d->pSpeed                         = new QLabel(QString::fromLatin1("1.00"));
    d->pSpeed->setContentsMargins(QMargins(1, 1, 1, 1));
    d->pSpeed->setToolTip(i18nc("@info", "Speed. Ctrl+Up/Down"));

    d->pPlayPauseBtn                  = new QToolButton(d->pControl);
    d->pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    d->pStopBtn                       = new QToolButton(d->pControl);
    d->pStopBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-stop")));
    d->pBackwardBtn                   = new QToolButton(d->pControl);
    d->pBackwardBtn->setIcon(QIcon::fromTheme(QLatin1String("media-skip-backward")));
    d->pForwardBtn                    = new QToolButton(d->pControl);
    d->pForwardBtn->setIcon(QIcon::fromTheme(QLatin1String("media-skip-forward")));
    d->pOpenBtn                       = new QToolButton(d->pControl);
    d->pOpenBtn->setToolTip(i18nc("@info: open new media", "Open"));
    d->pOpenBtn->setIcon(QIcon::fromTheme(QLatin1String("media-eject")));

    d->pInfoBtn                       = new QToolButton();
    d->pInfoBtn->setToolTip(QString::fromLatin1("Media information"));
    d->pInfoBtn->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    d->pCaptureBtn                    = new QToolButton();
    d->pCaptureBtn->setToolTip(i18nc("@info: button", "Capture"));
    d->pCaptureBtn->setIcon(QIcon::fromTheme(QLatin1String("media-record")));
    d->pVolumeBtn                     = new QToolButton();
    d->pVolumeBtn->setToolTip(i18nc("@info: button", "Volume"));
    d->pVolumeBtn->setIcon(QIcon::fromTheme(QLatin1String("player-volume")));

    d->pVolumeSlider                  = new AVPlayerSlider();
    d->pVolumeSlider->hide();
    d->pVolumeSlider->setOrientation(Qt::Horizontal);
    d->pVolumeSlider->setMinimum(0);
    const int kVolumeSliderMax      = 100;
    d->pVolumeSlider->setMaximum(kVolumeSliderMax);
/*
    d->pVolumeSlider->setMaximumHeight(12);
*/
    d->pVolumeSlider->setMaximumWidth(88);
    d->pVolumeSlider->setValue(int(1.0 / d->kVolumeInterval * qreal(kVolumeSliderMax) / 100.0));
    setVolume();

    d->pMenuBtn = new QToolButton();
    d->pMenuBtn->setIcon(QIcon::fromTheme(QLatin1String("application-menu")));
    d->pMenuBtn->setAutoRaise(true);
    d->pMenuBtn->setPopupMode(QToolButton::InstantPopup);

    QMenu* subMenu                  = nullptr;
    QWidgetAction* pWA              = nullptr;
    d->pMenu                          = new QMenu(d->pMenuBtn);
    d->pMenu->addAction(i18nc("@action", "Open Url"), this, SLOT(openUrl()));
    d->pMenu->addSeparator();

    QString appDataDir              = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    subMenu                         = new QMenu(i18nc("@option", "Play list"));
    d->pMenu->addMenu(subMenu);
    d->pPlayList                      = new PlayList(this);
    d->pPlayList->setSaveFile(appDataDir + QString::fromLatin1("/playlist.qds"));
    d->pPlayList->load();

    connect(d->pPlayList, SIGNAL(aboutToPlay(QString)),
            this, SLOT(play(QString)));

    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(d->pPlayList);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    subMenu                         = new QMenu(i18nc("@option", "History"));
    d->pMenu->addMenu(subMenu);
    d->pHistory                       = new PlayList(this);
    d->pHistory->setMaxRows(20);
    d->pHistory->setSaveFile(appDataDir + QString::fromLatin1("/history.qds"));
    d->pHistory->load();

    connect(d->pHistory, SIGNAL(aboutToPlay(QString)),
            this, SLOT(play(QString)));

    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(d->pHistory);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    d->pMenu->addSeparator();
/*
    d->pMenu->addAction(i18n("Report"))->setEnabled(false); // report bug, suggestions etc. using maillist?
*/
    d->pMenu->addAction(i18nc("@action", "About"), this, SLOT(about()));
    d->pMenu->addAction(i18nc("@action", "Setup"), this, SLOT(setup()));
    d->pMenu->addSeparator();
    d->pMenuBtn->setMenu(d->pMenu);
    d->pMenu->addSeparator();

    subMenu                         = new QMenu(i18nc("@option", "Speed"));
    d->pMenu->addMenu(subMenu);
    QDoubleSpinBox* const pSpeedBox = new QDoubleSpinBox(0);
    pSpeedBox->setRange(0.01, 20);
    pSpeedBox->setValue(1.0);
    pSpeedBox->setSingleStep(0.01);
    pSpeedBox->setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(pSpeedBox);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    subMenu                         = new ClickableMenu(i18nc("@option", "Repeat"));
    d->pMenu->addMenu(subMenu);
/*
    subMenu->setEnabled(false);
*/
    d->pRepeatEnableAction            = subMenu->addAction(i18nc("@action", "Enable"));
    d->pRepeatEnableAction->setCheckable(true);

    connect(d->pRepeatEnableAction, SIGNAL(toggled(bool)),
            this, SLOT(toggleRepeat(bool)));

    // TODO: move to a func or class

    d->pRepeatBox                     = new QSpinBox(0);
    d->pRepeatBox->setMinimum(-1);
    d->pRepeatBox->setValue(-1);
    d->pRepeatBox->setToolTip(QString::fromLatin1("-1: ") + i18nc("@info", "infinity"));
    connect(d->pRepeatBox, SIGNAL(valueChanged(int)), SLOT(setRepeateMax(int)));
    QLabel* pRepeatLabel            = new QLabel(i18nc("@label", "Times"));
    QHBoxLayout* hb                 = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(d->pRepeatBox);
    QVBoxLayout* const vb           = new QVBoxLayout;
    vb->addLayout(hb);
    pRepeatLabel                    = new QLabel(i18nc("@label: from time-stamp", "From"));
    d->pRepeatA                       = new QTimeEdit();
    d->pRepeatA->setDisplayFormat(QString::fromLatin1("HH:mm:ss"));
    d->pRepeatA->setToolTip(i18nc("@info", "negative value means from the end"));

    connect(d->pRepeatA, SIGNAL(timeChanged(QTime)),
            this, SLOT(repeatAChanged(QTime)));

    hb                              = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(d->pRepeatA);
    vb->addLayout(hb);
    pRepeatLabel                    = new QLabel(i18nc("@label: to time-stamp", "To"));
    d->pRepeatB                       = new QTimeEdit();
    d->pRepeatB->setDisplayFormat(QString::fromLatin1("HH:mm:ss"));
    d->pRepeatB->setToolTip(i18nc("@info", "negative value means from the end"));

    connect(d->pRepeatB, SIGNAL(timeChanged(QTime)),
            this, SLOT(repeatBChanged(QTime)));

    hb                              = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(d->pRepeatB);
    vb->addLayout(hb);
    QWidget* wgt                    = new QWidget;
    wgt->setLayout(vb);

    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    pWA->defaultWidget()->setEnabled(false);
    subMenu->addAction(pWA);                    // must add action after the widget action is ready. is it a Qt bug?
    d->pRepeatAction                  = pWA;

    d->pMenu->addSeparator();

    subMenu                         = new ClickableMenu(i18nc("@action", "Clock"));
    d->pMenu->addMenu(subMenu);
    QActionGroup* ag                = new QActionGroup(subMenu);
    ag->setExclusive(true);

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(changeClockType(QAction*)));

    subMenu->addAction(i18nc("@option: clock sync", "Auto"))->setData(-1);
    subMenu->addAction(i18nc("@option: clock sync", "Audio"))->setData(AVClock::AudioClock);
    subMenu->addAction(i18nc("@option: clock sync", "Video"))->setData(AVClock::VideoClock);

    Q_FOREACH (QAction* const action, subMenu->actions())
    {
        action->setActionGroup(ag);
        action->setCheckable(true);
    }

    QAction* const autoClockAction  = subMenu->actions().at(0);
    autoClockAction->setChecked(true);
    autoClockAction->setToolTip(i18nc("@info", "Take effect in next playback"));

    subMenu                         = new ClickableMenu(i18nc("@option", "Subtitle"));
    d->pMenu->addMenu(subMenu);
    QAction* act                    = subMenu->addAction(i18nc("@action", "Enable"));
    act->setCheckable(true);
    act->setChecked(d->pSubtitle->isEnabled());

    connect(act, SIGNAL(toggled(bool)),
            this, SLOT(toggoleSubtitleEnabled(bool)));

    act                             = subMenu->addAction(i18nc("@action", "Auto load"));
    act->setCheckable(true);
    act->setChecked(d->pSubtitle->autoLoad());

    connect(act, SIGNAL(toggled(bool)),
            this, SLOT(toggleSubtitleAutoLoad(bool)));

    subMenu->addAction(i18nc("@action: open subtitle file", "Open"), this, SLOT(openSubtitle()));

    wgt                             = new QWidget();
    hb                              = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(i18nc("@label", "Engine")));
    QComboBox* box                  = new QComboBox();
    hb->addWidget(box);
    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA);                    // must add action after the widget action is ready. is it a Qt bug?
    box->addItem(QString::fromLatin1("FFmpeg"), QString::fromLatin1("FFmpeg"));
    box->addItem(QString::fromLatin1("LibASS"), QString::fromLatin1("LibASS"));

    connect(box, SIGNAL(activated(int)),
            this, SLOT(setSubtitleEngine(int)));

    d->pSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
    box->setToolTip(i18nc("@info", "FFmpeg supports more subtitles but only render plain text\n"
                                   "LibASS supports 'ass' styles"));

    wgt                             = new QWidget();
    hb                              = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(i18nc("@label", "Charset")));
    box                             = new QComboBox();
    hb->addWidget(box);
    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA);                    // must add action after the widget action is ready. is it a Qt bug?
    box->addItem(i18nc("@option", "Auto detect"), QString::fromLatin1("AutoDetect"));
    box->addItem(i18nc("@option", "System"),      QString::fromLatin1("System"));

#if (QT_VERSION <= QT_VERSION_CHECK(5, 99, 0))
    Q_FOREACH (const QByteArray& cs, QTextCodec::availableCodecs())
    {
        box->addItem(QString::fromLatin1(cs), QString::fromLatin1(cs));
    }
#else
    // FIXME, QTextCodec is now in Qt5Compat
#endif

    connect(box, SIGNAL(activated(int)),
            this, SLOT(setSubtitleCharset(int)));

    d->pSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
    box->setToolTip(i18nc("@info","Auto detect requires libchardet"));

    subMenu                         = new ClickableMenu(i18nc("@option", "Audio track"));
    d->pMenu->addMenu(subMenu);
    d->pAudioTrackMenu              = subMenu;

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(changeAudioTrack(QAction*)));

    initAudioTrackMenu();

    subMenu                         = new ClickableMenu(i18nc("@option", "Channel"));
    d->pMenu->addMenu(subMenu);
    d->pChannelMenu                 = subMenu;

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(changeChannel(QAction*)));

    // will set to input in resampler if not supported.

    subMenu->addAction(i18nc("@action: channel option", "As input"))->setData(AudioFormat::ChannelLayout_Unsupported);
    subMenu->addAction(i18nc("@action: channel option", "Stereo"))->setData(AudioFormat::ChannelLayout_Stereo);
    subMenu->addAction(i18nc("@action: channel option", "Mono (center)"))->setData(AudioFormat::ChannelLayout_Center);
    subMenu->addAction(i18nc("@action: channel option", "Left"))->setData(AudioFormat::ChannelLayout_Left);
    subMenu->addAction(i18nc("@action: channel option", "Right"))->setData(AudioFormat::ChannelLayout_Right);
    ag                              = new QActionGroup(subMenu);
    ag->setExclusive(true);

    Q_FOREACH (QAction* const action, subMenu->actions())
    {
        ag->addAction(action);
        action->setCheckable(true);
    }

    subMenu                         = new QMenu(i18nc("@option", "Aspect ratio"), d->pMenu);
    d->pMenu->addMenu(subMenu);

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(switchAspectRatio(QAction*)));

    d->pARAction                      = subMenu->addAction(i18nc("@action: aspect ratio", "Video"));
    d->pARAction->setData(0);
    subMenu->addAction(i18nc("@action: video ratio", "Window"))->setData(-1);
    subMenu->addAction(QString::fromLatin1("4:3"))->setData(4.0   / 3.0);
    subMenu->addAction(QString::fromLatin1("16:9"))->setData(16.0 / 9.0);
    subMenu->addAction(i18nc("@action: video ratio", "Custom"))->setData(-2);

    Q_FOREACH (QAction* const action, subMenu->actions())
    {
        action->setCheckable(true);
    }

    d->pARAction->setChecked(true);

    subMenu                         = new ClickableMenu(i18nc("@option", "Color space"));
    d->pMenu->addMenu(subMenu);
    d->pVideoEQ                       = new VideoEQConfigPage();

    connect(d->pVideoEQ, SIGNAL(engineChanged()),
            this, SLOT(onVideoEQEngineChanged()));

    pWA                             = new QWidgetAction(nullptr);
    pWA->setDefaultWidget(d->pVideoEQ);
    subMenu->addAction(pWA);

    subMenu                         = new ClickableMenu(i18nc("@option", "Decoder"));
    d->pMenu->addMenu(subMenu);
    d->pDecoderConfigPage             = new DecoderConfigPage();
    pWA                             = new QWidgetAction(nullptr);
    pWA->setDefaultWidget(d->pDecoderConfigPage);
    subMenu->addAction(pWA);

    subMenu                         = new ClickableMenu(i18nc("@option", "Renderer"));
    d->pMenu->addMenu(subMenu);

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(changeVO(QAction*)));

    // TODO: AVOutput.name,detail(description). check whether it is available

    VideoRendererId* vo = nullptr;

    while ((vo = VideoRenderer::next(vo)))
    {
        // skip non-widget renderers

        if ((*vo == VideoRendererId_OpenGLWindow) || (*vo == VideoRendererId_GraphicsItem))
            continue;

        QAction* const voa = subMenu->addAction(QString::fromLatin1(VideoRenderer::name(*vo)));
        voa->setData(*vo);
        voa->setCheckable(true);

        if (!d->pVOAction)
            d->pVOAction = voa;
    }

    if (d->pVOAction)
        d->pVOAction->setChecked(true);
    d->VOActions                       = subMenu->actions();

    mainLayout->addLayout(d->pPlayerLayout);
    mainLayout->addWidget(d->pTimeSlider);
    mainLayout->addWidget(d->pControl);

    QHBoxLayout* const controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(0);
    controlLayout->setContentsMargins(QMargins(1, 1, 1, 1));
    d->pControl->setLayout(controlLayout);
    controlLayout->addWidget(d->pCurrent);
    controlLayout->addWidget(d->pTitle);

    QSpacerItem* const space         = new QSpacerItem(d->pPlayPauseBtn->width(),
                                                       d->pPlayPauseBtn->height(),
                                                       QSizePolicy::MinimumExpanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(d->pVolumeSlider);
    controlLayout->addWidget(d->pVolumeBtn);
    controlLayout->addWidget(d->pCaptureBtn);
    controlLayout->addWidget(d->pPlayPauseBtn);
    controlLayout->addWidget(d->pStopBtn);
    controlLayout->addWidget(d->pBackwardBtn);
    controlLayout->addWidget(d->pForwardBtn);
    controlLayout->addWidget(d->pOpenBtn);
    controlLayout->addWidget(d->pInfoBtn);
    controlLayout->addWidget(d->pSpeed);
/*
    controlLayout->addWidget(d->pSetupBtn);
*/
    controlLayout->addWidget(d->pMenuBtn);
    controlLayout->addWidget(d->pEnd);

    connect(pSpeedBox, SIGNAL(valueChanged(double)),
            this, SLOT(onSpinBoxChanged(double)));

    connect(d->pOpenBtn, SIGNAL(clicked()),
            this, SLOT(openFile()));

    connect(d->pPlayPauseBtn, SIGNAL(clicked()),
            this, SLOT(togglePlayPause()));

    connect(d->pInfoBtn, SIGNAL(clicked()),
            this, SLOT(showInfo()));

    // valueChanged can be triggered by non-mouse event

    connect(d->pTimeSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(seek(int)));

    connect(d->pTimeSlider, SIGNAL(sliderPressed()),
            this, SLOT(seek()));

    connect(d->pTimeSlider, SIGNAL(onLeave()),
            this, SLOT(onTimeSliderLeave()));

    connect(d->pTimeSlider, SIGNAL(onHover(int,int)),
            this, SLOT(onTimeSliderHover(int,int)));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(userShaderEnabledChanged()),
            this, SLOT(onUserShaderChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(intermediateFBOChanged()),
            this, SLOT(onUserShaderChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(fragHeaderChanged()),
            this, SLOT(onUserShaderChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(fragSampleChanged()),
            this, SLOT(onUserShaderChanged()));

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(fragPostProcessChanged()),
            this, SLOT(onUserShaderChanged()));

    QTimer::singleShot(0, this, SLOT(initPlayer()));
}

void MainWindow::setFrameRate()
{
    if (!d->pPlayer)
        return;

    d->pPlayer->setFrameRate(AVPlayerConfigMngr::instance().forceFrameRate());
}

void MainWindow::setVolume()
{
    AudioOutput* const ao = d->pPlayer ? d->pPlayer->audio() : nullptr;
    qreal v               = qreal(d->pVolumeSlider->value()) * d->kVolumeInterval;

    if (ao)
    {
        if (qAbs(int(ao->volume() / d->kVolumeInterval) - d->pVolumeSlider->value()) >= int(0.1 / d->kVolumeInterval))
        {
            ao->setVolume(v);
        }
    }

    d->pVolumeSlider->setToolTip(QString::number(v));
    d->pVolumeBtn->setToolTip(QString::number(v));
}

void MainWindow::setup()
{
    ConfigDialog::display();
}

void MainWindow::changeClockType(QAction* action)
{
    action->setChecked(true);
    int value = action->data().toInt();

    if (value < 0)
    {
        d->pPlayer->masterClock()->setClockAuto(true);

        // TODO: guess clock type

        return;
    }

    d->pPlayer->masterClock()->setClockAuto(false);
    d->pPlayer->masterClock()->setClockType(AVClock::ClockType(value));
}

void MainWindow::setRepeateMax(int m)
{
    d->RepeateMax = m;

    if (d->pPlayer)
    {
        d->pPlayer->setRepeat(m);
    }
}

void MainWindow::changeVO(QAction* action)
{
    if (action == d->pVOAction)
    {
        action->toggle(); // check state changes if clicked

        return;
    }

    VideoRendererId vid     = (VideoRendererId)action->data().toInt();
    VideoRenderer* const vo = VideoRenderer::create(vid);

    if (vo && vo->isAvailable())
    {
        if (!setRenderer(vo))
            action->toggle();
    }
    else
    {
        action->toggle(); // check state changes if clicked
        QMessageBox::critical(nullptr, QString::fromLatin1("AVPlayer"),
                              i18nc("@info", "not available on your platform!"));

        return;
    }
}

void MainWindow::changeChannel(QAction* action)
{
    if (action == d->pChannelAction)
    {
        action->toggle();

        return;
    }

    AudioFormat::ChannelLayout cl = (AudioFormat::ChannelLayout)action->data().toInt();
    AudioOutput* const ao         = d->pPlayer ? d->pPlayer->audio() : 0;                   // getAO() ?

    if (!ao)
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote()
            << QString::asprintf("No audio output!");

        return;
    }

    d->pChannelAction->setChecked(false);
    d->pChannelAction = action;
    d->pChannelAction->setChecked(true);

    if (!ao->close())
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote()
            << QString::asprintf("close audio failed");

        return;
    }

    AudioFormat af(ao->audioFormat());
    af.setChannelLayout(cl);
    ao->setAudioFormat(af);

    if (!ao->open())
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote()
            << QString::asprintf("open audio failed");

        return;
    }
}

void MainWindow::changeAudioTrack(QAction* action)
{
    int track = action->data().toInt();

    if ((d->pAudioTrackAction == action) && (track >= 0))
    {
        // external action is always clickable

        action->toggle();

        return;
    }

    if (track < 0)
    {
        QString f = QFileDialog::getOpenFileName(nullptr,
                                                 i18nc("@title:window",
                                                       "Open an External Audio Track"));

        if (f.isEmpty())
        {
            action->toggle();

            return;
        }

        d->pPlayer->setExternalAudio(f);
    }
    else
    {
        d->pPlayer->setExternalAudio(QString());

        if (!d->pPlayer->setAudioStream(track))
        {
            action->toggle();

            return;
        }
    }

    if (d->pAudioTrackAction)
        d->pAudioTrackAction->setChecked(false);

    d->pAudioTrackAction = action;
    d->pAudioTrackAction->setChecked(true);

    if (d->pStatisticsView && d->pStatisticsView->isVisible())
        d->pStatisticsView->setStatistics(d->pPlayer->statistics());
}

void MainWindow::setAudioBackends(const QStringList& backends)
{
    d->AudioBackends = backends;
}

bool MainWindow::setRenderer(QtAV::VideoRenderer* const renderer)
{
    if (!renderer)
        return false;

    if (!renderer->widget())
    {
        QMessageBox::warning(nullptr, QString::fromLatin1("AVPlayer"),
                             i18nc("@info", "Can not use this renderer"));

        return false;
    }

    d->pOSD->uninstall();
    d->pSubtitle->uninstall();
    renderer->widget()->setMouseTracking(true); // mouseMoveEvent without press.
    d->pPlayer->setRenderer(renderer);
    QWidget* r = nullptr;

    if (d->pRenderer)
        r = d->pRenderer->widget();

    // release old renderer and add new

    if (r)
    {
        d->pPlayerLayout->removeWidget(r);

        if (r->testAttribute(Qt::WA_DeleteOnClose))
        {
            r->close();
        }
        else
        {
            r->close();
            delete r;
        }

        r = nullptr;
    }

    d->pRenderer = renderer;

    // setInSize ?

    d->pPlayerLayout->addWidget(renderer->widget());

    if (d->pVOAction)
    {
        d->pVOAction->setChecked(false);
    }

    Q_FOREACH (QAction* const action, d->VOActions)
    {
        if (action->data() == renderer->id())
        {
            d->pVOAction = action;

            break;
        }
    }

    if (d->pVOAction)
    {
        d->pVOAction->setChecked(true);
        d->pTitle->setText(d->pVOAction->text());
    }

    const VideoRendererId vid = d->pPlayer->renderer()->id();

    if      ((vid == VideoRendererId_GLWidget)    ||
             (vid == VideoRendererId_GLWidget2)   ||
             (vid == VideoRendererId_OpenGLWidget)
       )
    {
        d->pVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale
                                                                   << VideoEQConfigPage::GLSL);
        d->pVideoEQ->setEngine(VideoEQConfigPage::GLSL);
        d->pPlayer->renderer()->forcePreferredPixelFormat(true);
    }
    else if (vid == VideoRendererId_XV)
    {
        d->pVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::XV);
        d->pVideoEQ->setEngine(VideoEQConfigPage::XV);
        d->pPlayer->renderer()->forcePreferredPixelFormat(true);
    }
    else
    {
        d->pVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale);
        d->pVideoEQ->setEngine(VideoEQConfigPage::SWScale);
        d->pPlayer->renderer()->forcePreferredPixelFormat(false);
    }

    onVideoEQEngineChanged();
    d->pOSD->installTo(d->pRenderer);
    d->pSubtitle->installTo(d->pRenderer);
    onUserShaderChanged();

#define GL_ASS 0

#if GL_ASS

    GLSLFilter* const glsl = new GLSLFilter(this);
    glsl->setOutputSize(QSize(4096, 2160));
/*
    d->pRenderer->installFilter(glsl);
*/
    if (d->pRenderer->opengl())
    {
        connect(d->pRenderer->opengl(), &OpenGLVideo::beforeRendering,
                [this]()
            {
                OpenGLVideo* const glv = d->pRenderer->opengl();
                glv->setSubImages(d->pSubtitle->subImages(glv->frameTime(), glv->frameWidth(), glv->frameHeight()));
            }
        );
    }

#endif

    return true;
}

void MainWindow::setVideoDecoderNames(const QStringList& vd)
{
    QStringList vdnames;

    Q_FOREACH (const QString& v, vd)
    {
        vdnames << v.toLower();
    }

    QStringList vidp;
    QStringList vids = DecoderConfigPage::idsToNames(VideoDecoder::registered());

    Q_FOREACH (const QString& v, vids)
    {
        if (vdnames.contains(v.toLower()))
        {
            vidp.append(v);
        }
    }

    AVPlayerConfigMngr::instance().setDecoderPriorityNames(vidp);
}

} // namespace AVPlayer
