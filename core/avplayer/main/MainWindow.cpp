/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "MainWindow.h"

// Qt includes

#include <QLocale>
#include <QTimer>
#include <QTimeEdit>
#include <QLabel>
#include <QApplication>
#include <QStandardPaths>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextStream>
#include <QUrl>
#include <QGraphicsOpacityEffect>
#include <QComboBox>
#include <QResizeEvent>
#include <QWidgetAction>
#include <QLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QToolTip>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QStyleFactory>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "EventFilter.h"
#include "QtAV.h"
#include "OpenGLVideo.h"
#include "VideoPreviewWidget.h"
#include "VideoShaderObject.h"
#include "ClickableMenu.h"
#include "AVPlayerSlider.h"
#include "StatisticsView.h"
#include "DecoderConfigPage.h"
#include "VideoEQConfigPage.h"
#include "ConfigDialog.h"
#include "OSDFilter.h"
#include "AVPlayerConfigMngr.h"
#include "ScreenSaver.h"
#include "PlayList.h"
#include "common.h"
#include "digikam_debug.h"

#ifndef QT_NO_OPENGL
#   include "GLSLFilter.h"
#endif

/*
 * TODO:
 * disable a/v actions if player is 0;
 * use action's value to set player's parameters when start to play a new file
 */

#define AVDEBUG()                               \
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()     \
        << QString::asprintf("%s %s @%d",       \
            __FILE__, __FUNCTION__, __LINE__);

using namespace QtAV;

namespace AVPlayer
{

const qreal kVolumeInterval = 0.04;

void QLabelSetElideText(QLabel* const label, const QString& text, int W = 0)
{
    QFontMetrics metrix(label->font());
    int width = label->width() - label->indent() - label->margin();

    if (W || label->parent())
    {
        int w = W;

        if (!w)
        {
            QWidget* const p = dynamic_cast<QWidget*>(label->parent());

            if (p)
                w = p->width();
        }

        width = qMax(w - label->indent() - label->margin() - 13 * (30), 0); // TODO: why 30?
    }

    QString clippedText = metrix.elidedText(text, Qt::ElideRight, width);
    label->setText(clippedText);
}

MainWindow::MainWindow(QWidget* const parent)
    : QWidget           (parent),
      m_IsReady          (false),
      m_HasPendingPlay   (false),
      m_ControlOn        (false),
      m_ShowControl      (2),
      m_RepeateMax       (0),
      m_pVOAction        (0),
      m_pPlayer          (0),
      m_pRenderer        (0),
      m_pVideoFilter     (0),
      m_pAudioFilter     (0),
      m_pStatisticsView  (0),
      m_pOSD             (0),
      m_pSubtitle        (0),
      m_preview         (0),
      m_shader          (nullptr),
      m_glsl            (nullptr)
{

#if defined(Q_OS_MACX) && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    QApplication::setStyle(QStyleFactory::create(QLatin1String("Fusion")));

#endif

    m_pOSD              = new OSDFilter(this);
    m_pSubtitle         = new SubtitleFilter(this);
    m_pChannelAction    = 0;
    m_pChannelMenu      = 0;
    m_pAudioTrackAction = 0;
    setMouseTracking(true); // mouseMoveEvent without press.

    connect(this, SIGNAL(ready()),
            this, SLOT(processPendingActions()));
/*
    QTimer::singleShot(10, this, SLOT(setupUi()));
*/
    setupUi();
/*
    setToolTip(i18n("Click black area to use shortcut (see right click menu)"));
*/
    WindowEventFilter* const we = new WindowEventFilter(this);
    installEventFilter(we);

    connect(we, SIGNAL(fullscreenChanged()),
            this, SLOT(handleFullscreenChange()));
}

MainWindow::~MainWindow()
{
    if (m_preview)
    {
        m_preview->close();
        delete m_preview;
    }

    m_pHistory->save();
    m_pPlayList->save();

    if (m_pVolumeSlider && !m_pVolumeSlider->parentWidget())
    {
        m_pVolumeSlider->close();
        delete m_pVolumeSlider;
        m_pVolumeSlider = nullptr;
    }

    if (m_pStatisticsView)
    {
        delete m_pStatisticsView;
        m_pStatisticsView = nullptr;
    }
}

void MainWindow::initPlayer()
{
    m_pPlayer                = new QtAV::AVPlayerCore(this);
    m_IsReady                = true;
    VideoRenderer* const vo = VideoRenderer::create((VideoRendererId)property("rendererId").toInt());

    if (!vo || !vo->isAvailable() || !vo->widget())
    {
        QMessageBox::critical(nullptr, QString::fromLatin1("AVPlayer"),
                              i18nc("@info", "Video renderer is not available on your platform!"));
    }

    setRenderer(vo);
/*
    m_pSubtitle->installTo(m_pPlayer); //filter on frame
*/
    m_pSubtitle->setPlayer(m_pPlayer);
/*
    m_pPlayer->setAudioOutput(AudioOutputFactory::create(AudioOutputId_OpenAL));
*/
    EventFilter* const ef = new EventFilter(m_pPlayer);
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

    connect(m_pStopBtn, SIGNAL(clicked()),
            this, SLOT(stopUnload()));

    connect(m_pForwardBtn, SIGNAL(clicked()),
             m_pPlayer, SLOT(seekForward()));

    connect(m_pBackwardBtn, SIGNAL(clicked()),
            m_pPlayer, SLOT(seekBackward()));

    connect(m_pVolumeBtn, SIGNAL(clicked()),
            this, SLOT(showHideVolumeBar()));

    connect(m_pVolumeSlider, SIGNAL(sliderPressed()),
            this, SLOT(setVolume()));

    connect(m_pVolumeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setVolume()));

    connect(m_pPlayer, SIGNAL(seekFinished(qint64)),
            this, SLOT(onSeekFinished(qint64)));

    connect(m_pPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),
            this, SLOT(onMediaStatusChanged()));

    connect(m_pPlayer, SIGNAL(bufferProgressChanged(qreal)),
            this, SLOT(onBufferProgress(qreal)));

    connect(m_pPlayer, SIGNAL(error(QtAV::AVError)),
            this, SLOT(handleError(QtAV::AVError)));

    connect(m_pPlayer, SIGNAL(started()),
            this, SLOT(onStartPlay()));

    connect(m_pPlayer, SIGNAL(stopped()),
            this, SLOT(onStopPlay()));

    connect(m_pPlayer, SIGNAL(paused(bool)),
            this, SLOT(onPaused(bool)));

    connect(m_pPlayer, SIGNAL(speedChanged(qreal)),
            this, SLOT(onSpeedChange(qreal)));

    connect(m_pPlayer, SIGNAL(positionChanged(qint64)),
            this, SLOT(onPositionChange(qint64)));
/*
    connect(m_pPlayer, SIGNAL(volumeChanged(qreal)),
            this, SLOT(syncVolumeUi(qreal)));
*/
    connect(m_pVideoEQ, SIGNAL(brightnessChanged(int)),
            this, SLOT(onBrightnessChanged(int)));

    connect(m_pVideoEQ, SIGNAL(contrastChanged(int)),
            this, SLOT(onContrastChanged(int)));

    connect(m_pVideoEQ, SIGNAL(hueChanegd(int)),
            this, SLOT(onHueChanged(int)));

    connect(m_pVideoEQ, SIGNAL(saturationChanged(int)),
            this, SLOT(onSaturationChanged(int)));

    connect(m_pCaptureBtn, SIGNAL(clicked()),
            m_pPlayer->videoCapture(), SLOT(capture()));

    // Emit this signal after connection.
    // otherwise the Q_SLOTS may not be called for the first time

    Q_EMIT ready();
}

void MainWindow::onSeekFinished(qint64 pos)
{
    qCDebug(DIGIKAM_AVPLAYER_LOG) << "seek finished at"
                                  << pos
                                  << "/"
                                  << m_pPlayer->position();
}

void MainWindow::stopUnload()
{
    m_pPlayer->stop();
}

void MainWindow::setupUi()
{
    QVBoxLayout* const mainLayout   = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(QMargins());
    setLayout(mainLayout);

    m_pPlayerLayout                  = new QVBoxLayout();
    m_pControl                       = new QWidget(this);
    m_pControl->setMaximumHeight(30);
/*
    m_pPreview = new QLabel(this);
*/
    m_pTimeSlider                    = new AVPlayerSlider(m_pControl);
    m_pTimeSlider->setDisabled(true);
    m_pTimeSlider->setTracking(true);
    m_pTimeSlider->setOrientation(Qt::Horizontal);
    m_pTimeSlider->setMinimum(0);
    m_pCurrent                       = new QLabel(m_pControl);
    m_pCurrent->setToolTip(i18nc("@info", "Current time"));
    m_pCurrent->setContentsMargins(QMargins(2, 2, 2, 2));
    m_pCurrent->setText(QString::fromLatin1("00:00:00"));
    m_pEnd                           = new QLabel(m_pControl);
    m_pEnd->setToolTip(i18nc("@info", "Duration"));
    m_pEnd->setContentsMargins(QMargins(2, 2, 2, 2));
    m_pEnd->setText(QString::fromLatin1("00:00:00"));
    m_pTitle                         = new QLabel(m_pControl);
    m_pTitle->setToolTip(i18nc("@info", "Render engine"));
    m_pTitle->setText(QString::fromLatin1("QPainter"));
    m_pTitle->setIndent(8);
    m_pSpeed                         = new QLabel(QString::fromLatin1("1.00"));
    m_pSpeed->setContentsMargins(QMargins(1, 1, 1, 1));
    m_pSpeed->setToolTip(i18nc("@info", "Speed. Ctrl+Up/Down"));

    m_pPlayPauseBtn                  = new QToolButton(m_pControl);
    m_pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    m_pStopBtn                       = new QToolButton(m_pControl);
    m_pStopBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-stop")));
    m_pBackwardBtn                   = new QToolButton(m_pControl);
    m_pBackwardBtn->setIcon(QIcon::fromTheme(QLatin1String("media-skip-backward")));
    m_pForwardBtn                    = new QToolButton(m_pControl);
    m_pForwardBtn->setIcon(QIcon::fromTheme(QLatin1String("media-skip-forward")));
    m_pOpenBtn                       = new QToolButton(m_pControl);
    m_pOpenBtn->setToolTip(i18nc("@info: open new media", "Open"));
    m_pOpenBtn->setIcon(QIcon::fromTheme(QLatin1String("media-eject")));

    m_pInfoBtn                       = new QToolButton();
    m_pInfoBtn->setToolTip(QString::fromLatin1("Media information"));
    m_pInfoBtn->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    m_pCaptureBtn                    = new QToolButton();
    m_pCaptureBtn->setToolTip(i18nc("@info: button", "Capture"));
    m_pCaptureBtn->setIcon(QIcon::fromTheme(QLatin1String("media-record")));
    m_pVolumeBtn                     = new QToolButton();
    m_pVolumeBtn->setToolTip(i18nc("@info: button", "Volume"));
    m_pVolumeBtn->setIcon(QIcon::fromTheme(QLatin1String("player-volume")));

    m_pVolumeSlider                  = new AVPlayerSlider();
    m_pVolumeSlider->hide();
    m_pVolumeSlider->setOrientation(Qt::Horizontal);
    m_pVolumeSlider->setMinimum(0);
    const int kVolumeSliderMax      = 100;
    m_pVolumeSlider->setMaximum(kVolumeSliderMax);
/*
    m_pVolumeSlider->setMaximumHeight(12);
*/
    m_pVolumeSlider->setMaximumWidth(88);
    m_pVolumeSlider->setValue(int(1.0 / kVolumeInterval * qreal(kVolumeSliderMax) / 100.0));
    setVolume();

    m_pMenuBtn = new QToolButton();
    m_pMenuBtn->setIcon(QIcon::fromTheme(QLatin1String("application-menu")));
    m_pMenuBtn->setAutoRaise(true);
    m_pMenuBtn->setPopupMode(QToolButton::InstantPopup);

    QMenu* subMenu                  = nullptr;
    QWidgetAction* pWA              = nullptr;
    m_pMenu                          = new QMenu(m_pMenuBtn);
    m_pMenu->addAction(i18nc("@action", "Open Url"), this, SLOT(openUrl()));
    m_pMenu->addSeparator();

    QString appDataDir              = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    subMenu                         = new QMenu(i18nc("@option", "Play list"));
    m_pMenu->addMenu(subMenu);
    m_pPlayList                      = new PlayList(this);
    m_pPlayList->setSaveFile(appDataDir + QString::fromLatin1("/playlist.qds"));
    m_pPlayList->load();

    connect(m_pPlayList, SIGNAL(aboutToPlay(QString)),
            this, SLOT(play(QString)));

    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(m_pPlayList);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    subMenu                         = new QMenu(i18nc("@option", "History"));
    m_pMenu->addMenu(subMenu);
    m_pHistory                       = new PlayList(this);
    m_pHistory->setMaxRows(20);
    m_pHistory->setSaveFile(appDataDir + QString::fromLatin1("/history.qds"));
    m_pHistory->load();

    connect(m_pHistory, SIGNAL(aboutToPlay(QString)),
            this, SLOT(play(QString)));

    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(m_pHistory);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    m_pMenu->addSeparator();
/*
    m_pMenu->addAction(i18n("Report"))->setEnabled(false); // report bug, suggestions etc. using maillist?
*/
    m_pMenu->addAction(i18nc("@action", "About"), this, SLOT(about()));
    m_pMenu->addAction(i18nc("@action", "Setup"), this, SLOT(setup()));
    m_pMenu->addSeparator();
    m_pMenuBtn->setMenu(m_pMenu);
    m_pMenu->addSeparator();

    subMenu                         = new QMenu(i18nc("@option", "Speed"));
    m_pMenu->addMenu(subMenu);
    QDoubleSpinBox* const pSpeedBox = new QDoubleSpinBox(0);
    pSpeedBox->setRange(0.01, 20);
    pSpeedBox->setValue(1.0);
    pSpeedBox->setSingleStep(0.01);
    pSpeedBox->setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(pSpeedBox);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    subMenu                         = new ClickableMenu(i18nc("@option", "Repeat"));
    m_pMenu->addMenu(subMenu);
/*
    subMenu->setEnabled(false);
*/
    m_pRepeatEnableAction            = subMenu->addAction(i18nc("@action", "Enable"));
    m_pRepeatEnableAction->setCheckable(true);

    connect(m_pRepeatEnableAction, SIGNAL(toggled(bool)),
            this, SLOT(toggleRepeat(bool)));

    // TODO: move to a func or class

    m_pRepeatBox                     = new QSpinBox(0);
    m_pRepeatBox->setMinimum(-1);
    m_pRepeatBox->setValue(-1);
    m_pRepeatBox->setToolTip(QString::fromLatin1("-1: ") + i18nc("@info", "infinity"));
    connect(m_pRepeatBox, SIGNAL(valueChanged(int)), SLOT(setRepeateMax(int)));
    QLabel* pRepeatLabel            = new QLabel(i18nc("@label", "Times"));
    QHBoxLayout* hb                 = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(m_pRepeatBox);
    QVBoxLayout* const vb           = new QVBoxLayout;
    vb->addLayout(hb);
    pRepeatLabel                    = new QLabel(i18nc("@label: from time-stamp", "From"));
    m_pRepeatA                       = new QTimeEdit();
    m_pRepeatA->setDisplayFormat(QString::fromLatin1("HH:mm:ss"));
    m_pRepeatA->setToolTip(i18nc("@info", "negative value means from the end"));

    connect(m_pRepeatA, SIGNAL(timeChanged(QTime)),
            this, SLOT(repeatAChanged(QTime)));

    hb                              = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(m_pRepeatA);
    vb->addLayout(hb);
    pRepeatLabel                    = new QLabel(i18nc("@label: to time-stamp", "To"));
    m_pRepeatB                       = new QTimeEdit();
    m_pRepeatB->setDisplayFormat(QString::fromLatin1("HH:mm:ss"));
    m_pRepeatB->setToolTip(i18nc("@info", "negative value means from the end"));

    connect(m_pRepeatB, SIGNAL(timeChanged(QTime)),
            this, SLOT(repeatBChanged(QTime)));

    hb                              = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(m_pRepeatB);
    vb->addLayout(hb);
    QWidget* wgt                    = new QWidget;
    wgt->setLayout(vb);

    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    pWA->defaultWidget()->setEnabled(false);
    subMenu->addAction(pWA);                    // must add action after the widget action is ready. is it a Qt bug?
    m_pRepeatAction                  = pWA;

    m_pMenu->addSeparator();

    subMenu                         = new ClickableMenu(i18nc("@action", "Clock"));
    m_pMenu->addMenu(subMenu);
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
    m_pMenu->addMenu(subMenu);
    QAction* act                    = subMenu->addAction(i18nc("@action", "Enable"));
    act->setCheckable(true);
    act->setChecked(m_pSubtitle->isEnabled());

    connect(act, SIGNAL(toggled(bool)),
            this, SLOT(toggoleSubtitleEnabled(bool)));

    act                             = subMenu->addAction(i18nc("@action", "Auto load"));
    act->setCheckable(true);
    act->setChecked(m_pSubtitle->autoLoad());

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

    connect(box, SIGNAL(activated(QString)),
            this, SLOT(setSubtitleEngine(QString)));

    m_pSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
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

    Q_FOREACH (const QByteArray& cs, QTextCodec::availableCodecs())
    {
        box->addItem(QString::fromLatin1(cs), QString::fromLatin1(cs));
    }

    connect(box, SIGNAL(activated(QString)),
            this, SLOT(setSubtitleCharset(QString)));

    m_pSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
    box->setToolTip(i18nc("@info","Auto detect requires libchardet"));

    subMenu                         = new ClickableMenu(i18nc("@option", "Audio track"));
    m_pMenu->addMenu(subMenu);
    m_pAudioTrackMenu                = subMenu;

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(changeAudioTrack(QAction*)));

    initAudioTrackMenu();

    subMenu                         = new ClickableMenu(i18nc("@option", "Channel"));
    m_pMenu->addMenu(subMenu);
    m_pChannelMenu                   = subMenu;

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

    subMenu                         = new QMenu(i18nc("@option", "Aspect ratio"), m_pMenu);
    m_pMenu->addMenu(subMenu);

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(switchAspectRatio(QAction*)));

    m_pARAction                      = subMenu->addAction(i18nc("@action: aspect ratio", "Video"));
    m_pARAction->setData(0);
    subMenu->addAction(i18nc("@action: video ratio", "Window"))->setData(-1);
    subMenu->addAction(QString::fromLatin1("4:3"))->setData(4.0   / 3.0);
    subMenu->addAction(QString::fromLatin1("16:9"))->setData(16.0 / 9.0);
    subMenu->addAction(i18nc("@action: video ratio", "Custom"))->setData(-2);

    Q_FOREACH (QAction* const action, subMenu->actions())
    {
        action->setCheckable(true);
    }

    m_pARAction->setChecked(true);

    subMenu                         = new ClickableMenu(i18nc("@option", "Color space"));
    m_pMenu->addMenu(subMenu);
    m_pVideoEQ                       = new VideoEQConfigPage();

    connect(m_pVideoEQ, SIGNAL(engineChanged()),
            this, SLOT(onVideoEQEngineChanged()));

    pWA                             = new QWidgetAction(nullptr);
    pWA->setDefaultWidget(m_pVideoEQ);
    subMenu->addAction(pWA);

    subMenu                         = new ClickableMenu(i18nc("@option", "Decoder"));
    m_pMenu->addMenu(subMenu);
    m_pDecoderConfigPage             = new DecoderConfigPage();
    pWA                             = new QWidgetAction(nullptr);
    pWA->setDefaultWidget(m_pDecoderConfigPage);
    subMenu->addAction(pWA);

    subMenu                         = new ClickableMenu(i18nc("@option", "Renderer"));
    m_pMenu->addMenu(subMenu);

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

        if (!m_pVOAction)
            m_pVOAction = voa;
    }

    m_pVOAction->setChecked(true);
    m_VOActions                       = subMenu->actions();

    mainLayout->addLayout(m_pPlayerLayout);
    mainLayout->addWidget(m_pTimeSlider);
    mainLayout->addWidget(m_pControl);

    QHBoxLayout* const controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(0);
    controlLayout->setContentsMargins(QMargins(1, 1, 1, 1));
    m_pControl->setLayout(controlLayout);
    controlLayout->addWidget(m_pCurrent);
    controlLayout->addWidget(m_pTitle);

    QSpacerItem* const space         = new QSpacerItem(m_pPlayPauseBtn->width(),
                                                       m_pPlayPauseBtn->height(),
                                                       QSizePolicy::MinimumExpanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(m_pVolumeSlider);
    controlLayout->addWidget(m_pVolumeBtn);
    controlLayout->addWidget(m_pCaptureBtn);
    controlLayout->addWidget(m_pPlayPauseBtn);
    controlLayout->addWidget(m_pStopBtn);
    controlLayout->addWidget(m_pBackwardBtn);
    controlLayout->addWidget(m_pForwardBtn);
    controlLayout->addWidget(m_pOpenBtn);
    controlLayout->addWidget(m_pInfoBtn);
    controlLayout->addWidget(m_pSpeed);
/*
    controlLayout->addWidget(m_pSetupBtn);
*/
    controlLayout->addWidget(m_pMenuBtn);
    controlLayout->addWidget(m_pEnd);

    connect(pSpeedBox, SIGNAL(valueChanged(double)),
            this, SLOT(onSpinBoxChanged(double)));

    connect(m_pOpenBtn, SIGNAL(clicked()),
            this, SLOT(openFile()));

    connect(m_pPlayPauseBtn, SIGNAL(clicked()),
            this, SLOT(togglePlayPause()));

    connect(m_pInfoBtn, SIGNAL(clicked()),
            this, SLOT(showInfo()));

    // valueChanged can be triggered by non-mouse event

    connect(m_pTimeSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(seek(int)));

    connect(m_pTimeSlider, SIGNAL(sliderPressed()),
            this, SLOT(seek()));

    connect(m_pTimeSlider, SIGNAL(onLeave()),
            this, SLOT(onTimeSliderLeave()));

    connect(m_pTimeSlider, SIGNAL(onHover(int,int)),
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

void MainWindow::changeChannel(QAction* action)
{
    if (action == m_pChannelAction)
    {
        action->toggle();

        return;
    }

    AudioFormat::ChannelLayout cl = (AudioFormat::ChannelLayout)action->data().toInt();
    AudioOutput* const ao         = m_pPlayer ? m_pPlayer->audio() : 0;                   // getAO() ?

    if (!ao)
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote()
            << QString::asprintf("No audio output!");

        return;
    }

    m_pChannelAction->setChecked(false);
    m_pChannelAction = action;
    m_pChannelAction->setChecked(true);

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

    if ((m_pAudioTrackAction == action) && (track >= 0))
    {
        // external action is always clickable

        action->toggle();

        return;
    }

    if (track < 0)
    {
        QString f = QFileDialog::getOpenFileName(nullptr,
                                                 i18nc("@title",
                                                       "Open an external audio track"));

        if (f.isEmpty())
        {
            action->toggle();

            return;
        }

        m_pPlayer->setExternalAudio(f);
    }
    else
    {
        m_pPlayer->setExternalAudio(QString());

        if (!m_pPlayer->setAudioStream(track))
        {
            action->toggle();

            return;
        }
    }

    if (m_pAudioTrackAction)
        m_pAudioTrackAction->setChecked(false);

    m_pAudioTrackAction = action;
    m_pAudioTrackAction->setChecked(true);

    if (m_pStatisticsView && m_pStatisticsView->isVisible())
        m_pStatisticsView->setStatistics(m_pPlayer->statistics());
}

void MainWindow::changeVO(QAction* action)
{
    if (action == m_pVOAction)
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

void MainWindow::processPendingActions()
{
    if (m_HasPendingPlay)
    {
        m_HasPendingPlay = false;
        play(m_File);
    }
}

void MainWindow::setAudioBackends(const QStringList& backends)
{
    m_AudioBackends = backends;
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

    m_pOSD->uninstall();
    m_pSubtitle->uninstall();
    renderer->widget()->setMouseTracking(true); // mouseMoveEvent without press.
    m_pPlayer->setRenderer(renderer);
    QWidget* r = nullptr;

    if (m_pRenderer)
        r = m_pRenderer->widget();

    // release old renderer and add new

    if (r)
    {
        m_pPlayerLayout->removeWidget(r);

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

    m_pRenderer = renderer;

    // setInSize ?

    m_pPlayerLayout->addWidget(renderer->widget());

    if (m_pVOAction)
    {
        m_pVOAction->setChecked(false);
    }

    Q_FOREACH (QAction* const action, m_VOActions)
    {
        if (action->data() == renderer->id())
        {
            m_pVOAction = action;

            break;
        }
    }

    if (m_pVOAction)
    {
        m_pVOAction->setChecked(true);
        m_pTitle->setText(m_pVOAction->text());
    }

    const VideoRendererId vid = m_pPlayer->renderer()->id();

    if      ((vid == VideoRendererId_GLWidget)    ||
             (vid == VideoRendererId_GLWidget2)   ||
             (vid == VideoRendererId_OpenGLWidget)
       )
    {
        m_pVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale
                                                                   << VideoEQConfigPage::GLSL);
        m_pVideoEQ->setEngine(VideoEQConfigPage::GLSL);
        m_pPlayer->renderer()->forcePreferredPixelFormat(true);
    }
    else if (vid == VideoRendererId_XV)
    {
        m_pVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::XV);
        m_pVideoEQ->setEngine(VideoEQConfigPage::XV);
        m_pPlayer->renderer()->forcePreferredPixelFormat(true);
    }
    else
    {
        m_pVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale);
        m_pVideoEQ->setEngine(VideoEQConfigPage::SWScale);
        m_pPlayer->renderer()->forcePreferredPixelFormat(false);
    }

    onVideoEQEngineChanged();
    m_pOSD->installTo(m_pRenderer);
    m_pSubtitle->installTo(m_pRenderer);
    onUserShaderChanged();

#define GL_ASS 0

#if GL_ASS

    GLSLFilter* const glsl = new GLSLFilter(this);
    glsl->setOutputSize(QSize(4096, 2160));
/*
    m_pRenderer->installFilter(glsl);
*/
    if (m_pRenderer->opengl())
    {
        connect(m_pRenderer->opengl(), &OpenGLVideo::beforeRendering,
                [this]()
            {
                OpenGLVideo* const glv = m_pRenderer->opengl();
                glv->setSubImages(m_pSubtitle->subImages(glv->frameTime(), glv->frameWidth(), glv->frameHeight()));
            }
        );
    }

#endif

    return true;
}

void MainWindow::play(const QString& name)
{
    m_File = name;

    if (!m_IsReady)
    {
        m_HasPendingPlay = true;

        return;
    }

    m_Title = m_File;

    if (!m_File.contains(QLatin1String("://")) || m_File.startsWith(QLatin1String("file://")))
    {
        m_Title = QFileInfo(m_File).fileName();
    }

    setWindowTitle(m_Title);
    m_pPlayer->stop();       // if no stop, in setPriority decoder will reopen
    m_pPlayer->setFrameRate(AVPlayerConfigMngr::instance().forceFrameRate());

    if (!m_AudioBackends.isEmpty())
        m_pPlayer->audio()->setBackends(m_AudioBackends);

    if (!m_pRepeatEnableAction->isChecked())
        m_RepeateMax = 0;

    m_pPlayer->setInterruptOnTimeout(AVPlayerConfigMngr::instance().abortOnTimeout());
    m_pPlayer->setInterruptTimeout(AVPlayerConfigMngr::instance().timeout() * 1000.0);
    m_pPlayer->setBufferMode(QtAV::BufferPackets);
    m_pPlayer->setBufferValue(AVPlayerConfigMngr::instance().bufferValue());
    m_pPlayer->setRepeat(m_RepeateMax);
    m_pPlayer->setPriority(DecoderConfigPage::idsFromNames(AVPlayerConfigMngr::instance().decoderPriorityNames()));
    m_pPlayer->setOptionsForAudioCodec(m_pDecoderConfigPage->audioDecoderOptions());
    m_pPlayer->setOptionsForVideoCodec(m_pDecoderConfigPage->videoDecoderOptions());

    if (AVPlayerConfigMngr::instance().avformatOptionsEnabled())
        m_pPlayer->setOptionsForFormat(AVPlayerConfigMngr::instance().avformatOptions());

    qCDebug(DIGIKAM_AVPLAYER_LOG) << AVPlayerConfigMngr::instance().avformatOptions();

    PlayListItem item;
    item.setUrl(m_File);
    item.setTitle(m_Title);
    item.setLastTime(0);
    m_pHistory->remove(m_File);
    m_pHistory->insertItemAt(item, 0);
    m_pPlayer->play(name);
}

void MainWindow::play(const QUrl& url)
{
    play(QUrl::fromPercentEncoding(url.toEncoded()));
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

void MainWindow::openFile()
{
    QString file = QFileDialog::getOpenFileName(nullptr, i18nc("@title", "Open a media file"),
                                                AVPlayerConfigMngr::instance().lastFile());

    if (file.isEmpty())
        return;

    AVPlayerConfigMngr::instance().setLastFile(file);
    play(file);
}

void MainWindow::togglePlayPause()
{
    if (m_pPlayer->isPlaying())
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("isPaused = %d", m_pPlayer->isPaused());
        m_pPlayer->pause(!m_pPlayer->isPaused());
    }
    else
    {
        if (m_File.isEmpty())
            return;

        if (!m_pPlayer->isPlaying())
            play(m_File);
        else
            m_pPlayer->play();

        m_pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("player-volume")));
    }
}

void MainWindow::showNextOSD()
{
    if (!m_pOSD)
        return;

    m_pOSD->useNextShowType();
}

void MainWindow::onSpinBoxChanged(double v)
{
    if (!m_pPlayer)
        return;

    m_pPlayer->setSpeed(v);
}

void MainWindow::onPaused(bool p)
{
    if (p)
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("start pausing...");
        m_pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    }
    else
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("stop pausing...");
        m_pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
    }
}

void MainWindow::onStartPlay()
{
    m_pRenderer->setRegionOfInterest(QRectF());
    m_File  = m_pPlayer->file();       // open from EventFilter's menu
    m_Title = m_File;

    if (!m_File.contains(QLatin1String("://")) || m_File.startsWith(QLatin1String("file://")))
        m_Title = QFileInfo(m_File).fileName();

    setWindowTitle(m_Title);

    m_pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
    m_pTimeSlider->setMinimum(m_pPlayer->mediaStartPosition());
    m_pTimeSlider->setMaximum(m_pPlayer->mediaStopPosition());
    m_pTimeSlider->setValue(0);
    m_pTimeSlider->setEnabled(m_pPlayer->isSeekable());
    m_pEnd->setText(QTime(0, 0, 0).addMSecs(m_pPlayer->mediaStopPosition()).toString(QString::fromLatin1("HH:mm:ss")));
    setVolume();
    m_ShowControl = 0;

    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));

    ScreenSaver::instance().disable();
    initAudioTrackMenu();
    m_pRepeatA->setMinimumTime(QTime(0, 0, 0).addMSecs(m_pPlayer->mediaStartPosition()));
    m_pRepeatA->setMaximumTime(QTime(0, 0, 0).addMSecs(m_pPlayer->mediaStopPosition()));
    m_pRepeatB->setMinimumTime(QTime(0, 0, 0).addMSecs(m_pPlayer->mediaStartPosition()));
    m_pRepeatB->setMaximumTime(QTime(0, 0, 0).addMSecs(m_pPlayer->mediaStopPosition()));
    m_pRepeatA->setTime(QTime(0, 0, 0).addMSecs(m_pPlayer->startPosition()));
    m_pRepeatB->setTime(QTime(0, 0, 0).addMSecs(m_pPlayer->stopPosition()));
    m_CursorTimer      = startTimer(3000);
    PlayListItem item = m_pHistory->itemAt(0);
    item.setUrl(m_File);
    item.setTitle(m_Title);
    item.setDuration(m_pPlayer->duration());
    m_pHistory->setItemAt(item, 0);
    updateChannelMenu();

    if (m_pStatisticsView && m_pStatisticsView->isVisible())
        m_pStatisticsView->setStatistics(m_pPlayer->statistics());
}

void MainWindow::onStopPlay()
{
    m_pPlayer->setPriority(DecoderConfigPage::idsFromNames(AVPlayerConfigMngr::instance().decoderPriorityNames()));

    if ((m_pPlayer->currentRepeat() >= 0) && (m_pPlayer->currentRepeat() < m_pPlayer->repeat()))
        return;

    // use shortcut to replay in EventFilter, the options will not be set, so set here

    m_pPlayer->setFrameRate(AVPlayerConfigMngr::instance().forceFrameRate());
    m_pPlayer->setOptionsForAudioCodec(m_pDecoderConfigPage->audioDecoderOptions());
    m_pPlayer->setOptionsForVideoCodec(m_pDecoderConfigPage->videoDecoderOptions());

    if (AVPlayerConfigMngr::instance().avformatOptionsEnabled())
        m_pPlayer->setOptionsForFormat(AVPlayerConfigMngr::instance().avformatOptions());

    m_pPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    m_pTimeSlider->setValue(0);

    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("disable slider");

    m_pTimeSlider->setDisabled(true);
    m_pTimeSlider->setMinimum(0);
    m_pTimeSlider->setMaximum(0);
    m_pCurrent->setText(QString::fromLatin1("00:00:00"));
    m_pEnd->setText(QString::fromLatin1("00:00:00"));
    tryShowControlBar();
    ScreenSaver::instance().enable();
    toggleRepeat(false);
/*
    m_RepeateMax = 0;
*/
    killTimer(m_CursorTimer);
    unsetCursor();

    if (m_preview)
        m_preview->setFile(QString());
}

void MainWindow::onSpeedChange(qreal speed)
{
    m_pSpeed->setText(QString::fromLatin1("%1").arg(speed, 4, 'f', 2, QLatin1Char('0')));
}

void MainWindow::setFrameRate()
{
    if (!m_pPlayer)
        return;

    m_pPlayer->setFrameRate(AVPlayerConfigMngr::instance().forceFrameRate());
}

void MainWindow::seek(int value)
{
    m_pPlayer->setSeekType(AccurateSeek);
    m_pPlayer->seek((qint64)value);

    if (!m_preview || !AVPlayerConfigMngr::instance().previewEnabled())
        return;

    m_preview->setTimestamp(value);
    m_preview->preview();
    m_preview->setWindowFlags(m_preview->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_preview->resize(AVPlayerConfigMngr::instance().previewWidth(), AVPlayerConfigMngr::instance().previewHeight());
    m_preview->show();
}

void MainWindow::seek()
{
    seek(m_pTimeSlider->value());
}

void MainWindow::showHideVolumeBar()
{
    if (m_pVolumeSlider->isHidden())
    {
        m_pVolumeSlider->show();
    }
    else
    {
        m_pVolumeSlider->hide();
    }
}

void MainWindow::setVolume()
{
    AudioOutput* const ao = m_pPlayer ? m_pPlayer->audio() : nullptr;
    qreal v               = qreal(m_pVolumeSlider->value()) * kVolumeInterval;

    if (ao)
    {
        if (qAbs(int(ao->volume() / kVolumeInterval) - m_pVolumeSlider->value()) >= int(0.1 / kVolumeInterval))
        {
            ao->setVolume(v);
        }
    }

    m_pVolumeSlider->setToolTip(QString::number(v));
    m_pVolumeBtn->setToolTip(QString::number(v));
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    Q_UNUSED(e);

    if (m_pPlayer)
        m_pPlayer->stop();

    qApp->quit();
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    QWidget::resizeEvent(e);
/*
    if (m_pTitle)
        QLabelSetElideText(m_pTitle, QFileInfo(m_File).fileName(), e->size().width());
*/
}

void MainWindow::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == m_CursorTimer)
    {
        if (m_pControl->isVisible())
            return;

        setCursor(Qt::BlankCursor);
    }
}

void MainWindow::onPositionChange(qint64 pos)
{
    if (m_pPlayer->isSeekable())
        m_pTimeSlider->setValue(pos);

    m_pCurrent->setText(QTime(0, 0, 0).addMSecs(pos).toString(QString::fromLatin1("HH:mm:ss")));
/*
    setWindowTitle(QString::number(m_pPlayer->statistics().video_only.currentDisplayFPS(),
                                   'f', 2).append(" ").append(m_Title));
*/
}

void MainWindow::repeatAChanged(const QTime& t)
{
    if (!m_pPlayer)
        return;

    m_pPlayer->setStartPosition(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::repeatBChanged(const QTime& t)
{
    if (!m_pPlayer)
        return;

    // when this slot is called? even if only range is set?

    if (t <= m_pRepeatA->time())
        return;

    m_pPlayer->setStopPosition(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    m_ControlOn = (e->key() == Qt::Key_Control);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e)
{
    Q_UNUSED(e);
    m_ControlOn = false;
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    if (!m_ControlOn)
        return;

    m_GlobalMouse = e->globalPos();
}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
    unsetCursor();

    if (e->pos().y() > (height() - m_pTimeSlider->height() - m_pControl->height()))
    {
        if (m_ShowControl == 0)
        {
            m_ShowControl = 1;
            tryShowControlBar();
        }
    }
    else
    {
        if (m_ShowControl == 1)
        {
            m_ShowControl = 0;
            QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
        }
    }

    if (m_ControlOn && (e->button() == Qt::LeftButton))
    {
        if (!m_pRenderer || !m_pRenderer->widget())
            return;

        QRectF roi    = m_pRenderer->realROI();
        QPointF delta = e->pos() - m_GlobalMouse;
        roi.moveLeft(-delta.x());
        roi.moveTop(-delta.y());
        m_pRenderer->setRegionOfInterest(roi);
    }
}

void MainWindow::wheelEvent(QWheelEvent* e)
{
    if (!m_pRenderer || !m_pRenderer->widget())
    {
        return;
    }

    QPoint dp;

    qreal deg = e->angleDelta().y() / 8.0;
    dp        = e->pixelDelta();

#ifdef WHEEL_SPEED

    if (!m_ControlOn)
    {
        qreal speed = m_pPlayer->speed();
        m_pPlayer->setSpeed(qMax(0.01, speed + deg / 15.0*0.02));

        return;
    }

#endif // WHEEL_SPEED

    QPointF p  = m_pRenderer->widget()->mapFrom(this, e->position().toPoint());
    QPointF fp = m_pRenderer->mapToFrame(p);
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

    if (fp.x() > m_pRenderer->videoFrameSize().width())
    {
        fp.setX(m_pRenderer->videoFrameSize().width());
    }

    if (fp.y() > m_pRenderer->videoFrameSize().height())
    {
        fp.setY(m_pRenderer->videoFrameSize().height());
    }

    QRectF viewport = QRectF(m_pRenderer->mapToFrame(QPointF(0, 0)),
                             m_pRenderer->mapToFrame(QPointF(m_pRenderer->rendererWidth(),
                             m_pRenderer->rendererHeight())));

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
                                  << QRectF(x0, y0, qreal(m_pRenderer->videoFrameSize().width()) / z,
                                                          qreal(m_pRenderer->videoFrameSize().height()) / z)
                                  << fp << z;
*/
    m_pRenderer->setRegionOfInterest(QRectF(x0, y0,
                                           qreal(m_pRenderer->videoFrameSize().width())  / z,
                                           qreal(m_pRenderer->videoFrameSize().height()) / z));

/*
    QTransform m;
    m.translate(fp.x(), fp.y());
    m.scale(1.0/zoom, 1.0/zoom);
    m.translate(-fp.x(), -fp.y());
    QRectF r = m.mapRect(m_pRenderer->realROI());
    m_pRenderer->setRegionOfInterest((r | m.mapRect(viewport))&QRectF(QPointF(0,0), m_pRenderer->videoFrameSize()));
*/
}

void MainWindow::about()
{
    QtAV::about();
}

void MainWindow::openUrl()
{
    QString url = QInputDialog::getText(nullptr, i18nc("@title", "Open an url"),
                                        i18nc("@info", "Url"));

    if (url.isEmpty())
        return;

    play(url);
}

void MainWindow::updateChannelMenu()
{
    if (m_pChannelAction)
        m_pChannelAction->setChecked(false);

    AudioOutput* const ao = (m_pPlayer ? m_pPlayer->audio() : nullptr); // getAO() ?

    if (!ao)
    {
        return;
    }

    AudioFormat::ChannelLayout cl = ao->audioFormat().channelLayout();
    QList<QAction*> as            = m_pChannelMenu->actions();

    Q_FOREACH (QAction* const action, as)
    {
        if (action->data().toInt() != (int)cl)
            continue;

        action->setChecked(true);
        m_pChannelAction = action;

        break;
    }
}

void MainWindow::initAudioTrackMenu()
{
    int track  = -2;
    QAction* a = nullptr;
    QList<QAction*> as;
    int tracks = 0;

    if (!m_pPlayer)
    {
        a = m_pAudioTrackMenu->addAction(i18nc("@action: audio track", "External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        m_pAudioTrackAction = 0;

        goto end;
    }

    track  = m_pPlayer->currentAudioStream();
    as     = m_pAudioTrackMenu->actions();
    tracks = m_pPlayer->audioStreamCount();

    if (m_pAudioTrackAction && ((tracks == as.size() - 1)) && (m_pAudioTrackAction->data().toInt() == track))
        return;

    while ((tracks + 1) < as.size())
    {
        a = as.takeLast();
        m_pAudioTrackMenu->removeAction(a);
        delete a;
    }

    if (as.isEmpty())
    {
        a = m_pAudioTrackMenu->addAction(i18nc("@action: audio track", "External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        m_pAudioTrackAction = 0;
    }

    while ((tracks + 1) > as.size())
    {
        a = m_pAudioTrackMenu->addAction(QString::number(as.size()-1));
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
            if (m_pPlayer && m_pPlayer->externalAudio().isEmpty())
            {
                qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                    << QString::asprintf("track found!");

                m_pAudioTrackAction = ac;
                ac->setChecked(true);
            }
        }
        else
        {
            ac->setChecked(false);
        }
    }

    if (m_pPlayer && !m_pPlayer->externalAudio().isEmpty())
    {
        m_pAudioTrackAction = as.first();
    }

    if (m_pAudioTrackAction)
        m_pAudioTrackAction->setChecked(true);
}

void MainWindow::switchAspectRatio(QAction *action)
{
    qreal r = action->data().toDouble();

    if ((action == m_pARAction) && (r != -2))
    {
        action->toggle(); // check state changes if clicked

        return;
    }

    if      (r == 0)
    {
        m_pPlayer->renderer()->setOutAspectRatioMode(QtAV::VideoAspectRatio);
    }
    else if (r == -1)
    {
        m_pPlayer->renderer()->setOutAspectRatioMode(QtAV::RendererAspectRatio);
    }
    else
    {
        if (r == -2)
            r = QInputDialog::getDouble(0, i18nc("@option", "Aspect ratio"), QString(), 1.0);

        m_pPlayer->renderer()->setOutAspectRatioMode(QtAV::CustomAspectRation);
        m_pPlayer->renderer()->setOutAspectRatio(r);
    }

    m_pARAction->setChecked(false);
    m_pARAction = action;
    m_pARAction->setChecked(true);
}

void MainWindow::toggleRepeat(bool r)
{
    m_pRepeatEnableAction->setChecked(r);
    m_pRepeatAction->defaultWidget()->setEnabled(r); // why need defaultWidget?

    if (r)
    {
        m_RepeateMax = m_pRepeatBox->value();
    }
    else
    {
        m_RepeateMax = 0;
    }

    if (m_pPlayer)
    {
        m_pPlayer->setRepeat(m_RepeateMax);

        if (r)
        {
            repeatAChanged(m_pRepeatA->time());
            repeatBChanged(m_pRepeatB->time());
        }
        else
        {
            m_pPlayer->setTimeRange(0);
        }
    }
}

void MainWindow::setRepeateMax(int m)
{
    m_RepeateMax = m;

    if (m_pPlayer)
    {
        m_pPlayer->setRepeat(m);
    }
}

void MainWindow::playOnlineVideo(QAction* action)
{
    m_Title = action->text();
    play(action->data().toString());
}

void MainWindow::onPlayListClick(const QString& key, const QString& value)
{
    m_Title = key;
    play(value);
}

void MainWindow::tryHideControlBar()
{
    if (m_ShowControl > 0)
    {
        return;
    }

    if (m_pControl->isHidden() && m_pTimeSlider->isHidden())
        return;

    m_pControl->hide();
    m_pTimeSlider->hide();
    workaroundRendererSize();
}

void MainWindow::tryShowControlBar()
{
    unsetCursor();

    if (m_pTimeSlider && m_pTimeSlider->isHidden())
        m_pTimeSlider->show();

    if (m_pControl && m_pControl->isHidden())
        m_pControl->show();
}

void MainWindow::showInfo()
{
    if (!m_pStatisticsView)
        m_pStatisticsView = new StatisticsView();

    if (m_pPlayer)
        m_pStatisticsView->setStatistics(m_pPlayer->statistics());

    m_pStatisticsView->show();
}

void MainWindow::onTimeSliderHover(int pos, int value)
{
    QPoint gpos = mapToGlobal(m_pTimeSlider->pos() + QPoint(pos, 0));
    QToolTip::showText(gpos, QTime(0, 0, 0).addMSecs(value).toString(QString::fromLatin1("HH:mm:ss")));

    if (!AVPlayerConfigMngr::instance().previewEnabled())
        return;

    if (!m_preview)
        m_preview = new VideoPreviewWidget();

    m_preview->setFile(m_pPlayer->file());
    m_preview->setTimestamp(value);
    m_preview->preview();
    const int w = AVPlayerConfigMngr::instance().previewWidth();
    const int h = AVPlayerConfigMngr::instance().previewHeight();
    m_preview->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_preview->resize(w, h);
    m_preview->move(gpos - QPoint(w / 2, h));
    m_preview->show();
}

void MainWindow::onTimeSliderLeave()
{
/*
    if (m_preview && m_preview->isVisible())
        m_preview->hide();
*/
    if (!m_preview)
    {
        return;
    }

    if (m_preview->isVisible())
    {
        m_preview->close();
    }

    delete m_preview;
    m_preview = nullptr;
}

void MainWindow::handleError(const AVError& e)
{
    QMessageBox::warning(nullptr, i18nc("@title", "Player error"), e.string());
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
    setWindowTitle(status + QString::fromLatin1(" ") + m_Title);
}

void MainWindow::onBufferProgress(qreal percent)
{
    const qreal bs = m_pPlayer->bufferSpeed();
    QString s;

    if      (bs > 1024 * 1024 * 1024)
        s = QString::fromLatin1("%1G/s").arg(bs / 1024.0 / 1024.0 / 1024.0, 6, 'f', 1);
    else if (bs > 1024*1024)
        s = QString::fromLatin1("%1M/s").arg(bs / 1024.0 / 1024.0, 6, 'f', 1);
    else if (bs > 1024)
        s = QString::fromLatin1("%1K/s").arg(bs / 1024.0, 6, 'f', 1);
    else
        s = QString::fromLatin1("%1B/s").arg(bs, 6, 'f', 1);

    setWindowTitle(i18nc("@title: media loading state", "Buffering... %1% @%2 %3",
                   QString::fromLatin1("%1").arg(percent*100.0, 5, 'f', 1), s, m_Title));
}

void MainWindow::onVideoEQEngineChanged()
{
    VideoRenderer* const vo     = m_pPlayer->renderer();
    VideoEQConfigPage::Engine e = m_pVideoEQ->engine();

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

    onBrightnessChanged(m_pVideoEQ->brightness() * 100.0);
    onContrastChanged(m_pVideoEQ->contrast() * 100.0);
    onHueChanged(m_pVideoEQ->hue() * 100.0);
    onSaturationChanged(m_pVideoEQ->saturation() * 100.0);
}

void MainWindow::onBrightnessChanged(int b)
{
    VideoRenderer* const vo = m_pPlayer->renderer();

    if ((m_pVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setBrightness(m_pVideoEQ->brightness()))
    {
        m_pPlayer->setBrightness(0);
    }
    else
    {
        vo->setBrightness(0);
        m_pPlayer->setBrightness(b);
    }
}

void MainWindow::onContrastChanged(int c)
{
    VideoRenderer* const vo = m_pPlayer->renderer();

    if ((m_pVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setContrast(m_pVideoEQ->contrast()))
    {
        m_pPlayer->setContrast(0);
    }
    else
    {
        vo->setContrast(0);
        m_pPlayer->setContrast(c);
    }
}

void MainWindow::onHueChanged(int h)
{
    VideoRenderer* const vo = m_pPlayer->renderer();

    if ((m_pVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setHue(m_pVideoEQ->hue()))
    {
        m_pPlayer->setHue(0);
    }
    else
    {
        vo->setHue(0);
        m_pPlayer->setHue(h);
    }
}

void MainWindow::onSaturationChanged(int s)
{
    VideoRenderer* const vo = m_pPlayer->renderer();

    if ((m_pVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setSaturation(m_pVideoEQ->saturation()))
    {
        m_pPlayer->setSaturation(0);
    }
    else
    {
        vo->setSaturation(0);
        m_pPlayer->setSaturation(s);
    }
}

void MainWindow::onCaptureConfigChanged()
{
    m_pPlayer->videoCapture()->setCaptureDir(AVPlayerConfigMngr::instance().captureDir());
    m_pPlayer->videoCapture()->setQuality(AVPlayerConfigMngr::instance().captureQuality());

    if (AVPlayerConfigMngr::instance().captureFormat().toLower() == QLatin1String("original"))
    {
        m_pPlayer->videoCapture()->setOriginalFormat(true);
    }
    else
    {
        m_pPlayer->videoCapture()->setOriginalFormat(false);
        m_pPlayer->videoCapture()->setSaveFormat(AVPlayerConfigMngr::instance().captureFormat());
    }

    m_pCaptureBtn->setToolTip(i18nc("@info", "Capture video frame\nSave to: %1\nFormat: %2",
                             m_pPlayer->videoCapture()->captureDir(),
                             AVPlayerConfigMngr::instance().captureFormat()));
}

void MainWindow::onAVFilterVideoConfigChanged()
{
    if (m_pVideoFilter)
    {
        m_pVideoFilter->uninstall();
        delete m_pVideoFilter;
        m_pVideoFilter = nullptr;
    }

    m_pVideoFilter = new LibAVFilterVideo(this);
    m_pVideoFilter->setEnabled(AVPlayerConfigMngr::instance().avfilterVideoEnable());
    m_pPlayer->installFilter(m_pVideoFilter);
    m_pVideoFilter->setOptions(AVPlayerConfigMngr::instance().avfilterVideoOptions());
}

void MainWindow::onAVFilterAudioConfigChanged()
{
    if (m_pAudioFilter)
    {
        m_pAudioFilter->uninstall();
        delete m_pAudioFilter;
        m_pAudioFilter = nullptr;
    }

    m_pAudioFilter = new LibAVFilterAudio(this);
    m_pAudioFilter->setEnabled(AVPlayerConfigMngr::instance().avfilterAudioEnable());
    m_pAudioFilter->installTo(m_pPlayer);
    m_pAudioFilter->setOptions(AVPlayerConfigMngr::instance().avfilterAudioOptions());
}

void MainWindow::onBufferValueChanged()
{
    if (!m_pPlayer)
        return;

    m_pPlayer->setBufferValue(AVPlayerConfigMngr::instance().bufferValue());
}

void MainWindow::onAbortOnTimeoutChanged()
{
    if (!m_pPlayer)
        return;

    m_pPlayer->setInterruptOnTimeout(AVPlayerConfigMngr::instance().abortOnTimeout());
}

void MainWindow::onUserShaderChanged()
{
    if (!m_pRenderer || !m_pRenderer->opengl())
        return;

#ifndef QT_NO_OPENGL

    if (AVPlayerConfigMngr::instance().userShaderEnabled())
    {
        if (AVPlayerConfigMngr::instance().intermediateFBO())
        {
            if (!m_glsl)
                m_glsl = new GLSLFilter(this);

            m_glsl->installTo(m_pRenderer);
        }
        else
        {
            if (m_glsl)
                m_glsl->uninstall();
        }

        if (!m_shader)
            m_shader = new DynamicShaderObject(this);

        m_shader->setHeader(AVPlayerConfigMngr::instance().fragHeader());
        m_shader->setSample(AVPlayerConfigMngr::instance().fragSample());
        m_shader->setPostProcess(AVPlayerConfigMngr::instance().fragPostProcess());
        m_pRenderer->opengl()->setUserShader(m_shader);
    }
    else
    {
        m_pRenderer->opengl()->setUserShader(nullptr);
    }

#endif

}

void MainWindow::setup()
{
    ConfigDialog::display();
}

void MainWindow::handleFullscreenChange()
{
    workaroundRendererSize();

    // workaround renderer display size for ubuntu

    tryShowControlBar();

    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
}

void MainWindow::toggoleSubtitleEnabled(bool value)
{
    m_pSubtitle->setEnabled(value);
}

void MainWindow::toggleSubtitleAutoLoad(bool value)
{
    m_pSubtitle->setAutoLoad(value);
}

void MainWindow::openSubtitle()
{
    QString file = QFileDialog::getOpenFileName(nullptr, i18nc("@title", "Open a subtitle file"));

    if (file.isEmpty())
        return;

    m_pSubtitle->setFile(file);
}

void MainWindow::setSubtitleCharset(const QString& charSet)
{
    Q_UNUSED(charSet);

    QComboBox* const box = qobject_cast<QComboBox*>(sender());

    if (!box)
        return;

    m_pSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
}

void MainWindow::setSubtitleEngine(const QString &value)
{
    Q_UNUSED(value)

    QComboBox* const box = qobject_cast<QComboBox*>(sender());

    if (!box)
        return;

    m_pSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
}

void MainWindow::changeClockType(QAction* action)
{
    action->setChecked(true);
    int value = action->data().toInt();

    if (value < 0)
    {
        m_pPlayer->masterClock()->setClockAuto(true);

        // TODO: guess clock type

        return;
    }

    m_pPlayer->masterClock()->setClockAuto(false);
    m_pPlayer->masterClock()->setClockType(AVClock::ClockType(value));
}

void MainWindow::syncVolumeUi(qreal value)
{
    const int v(value / kVolumeInterval);

    if (m_pVolumeSlider->value() == v)
        return;

    m_pVolumeSlider->setValue(v);
}

void MainWindow::workaroundRendererSize()
{
    if (!m_pRenderer)
        return;

    QSize s = rect().size();
/*
    resize(QSize(s.width()-1, s.height()-1));
    resize(s); // window resize to fullscreen size will create another fullScreenChange event
*/
    m_pRenderer->widget()->resize(QSize(s.width() + 1, s.height() + 1));
    m_pRenderer->widget()->resize(s);
}

} // namespace AVPlayer
