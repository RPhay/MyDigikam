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
#include "VideoShaderObject.h"
#include "QtAVWidgets.h"
#include "ClickableMenu.h"
#include "Slider.h"
#include "StatisticsView.h"
#include "DecoderConfigPage.h"
#include "VideoEQConfigPage.h"
#include "ConfigDialog.h"
#include "OSDFilter.h"

//#include "AVFilterSubtitle.h"

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

#define AVDEBUG() \
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("%s %s @%d", __FILE__, __FUNCTION__, __LINE__);

using namespace QtAV;

namespace AVPlayer
{

const qreal kVolumeInterval = 0.04;

extern QStringList             idsToNames(QVector<VideoDecoderId> ids);
extern QVector<VideoDecoderId> idsFromNames(const QStringList& names);

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

        width = qMax(w - label->indent() - label->margin() - 13*(30), 0); // TODO: why 30?
    }

    QString clippedText = metrix.elidedText(text, Qt::ElideRight, width);
    label->setText(clippedText);
}

MainWindow::MainWindow(QWidget* const parent)
    : QWidget(parent),
      mIsReady(false),
      mHasPendingPlay(false),
      mControlOn(false),
      mShowControl(2),
      mRepeateMax(0),
      mpVOAction(0),
      mpPlayer(0),
      mpRenderer(0),
      mpVideoFilter(0),
      mpAudioFilter(0),
      mpStatisticsView(0),
      mpOSD(0),
      mpSubtitle(0),
      m_preview(0),
      m_shader(nullptr),
      m_glsl(nullptr)
{

#if defined(Q_OS_MACX) && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    QApplication::setStyle(QStyleFactory::create(QLatin1String("Fusion")));

#endif

    mpOSD              = new OSDFilter(this);
    mpSubtitle         = new SubtitleFilter(this);
    mpChannelAction    = 0;
    mpChannelMenu      = 0;
    mpAudioTrackAction = 0;
    setMouseTracking(true); // mouseMoveEvent without press.

    connect(this, SIGNAL(ready()),
            this, SLOT(processPendingActions()));

    //QTimer::singleShot(10, this, SLOT(setupUi()));

    setupUi();

    //setToolTip(i18n("Click black area to use shortcut(see right click menu)"));

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

    mpHistory->save();
    mpPlayList->save();

    if (mpVolumeSlider && !mpVolumeSlider->parentWidget())
    {
        mpVolumeSlider->close();
        delete mpVolumeSlider;
        mpVolumeSlider = nullptr;
    }

    if (mpStatisticsView)
    {
        delete mpStatisticsView;
        mpStatisticsView = nullptr;
    }
}

void MainWindow::initPlayer()
{
    mpPlayer                = new QtAV::AVPlayerCore(this);
    mIsReady                = true;
    VideoRenderer* const vo = VideoRenderer::create((VideoRendererId)property("rendererId").toInt());

    if (!vo || !vo->isAvailable() || !vo->widget())
    {
        QMessageBox::critical(nullptr, QString::fromLatin1("AVPlayer"),
                              i18nc("@info", "Video renderer is not availabe on your platform!"));
    }

    setRenderer(vo);

    //mpSubtitle->installTo(mpPlayer); //filter on frame

    mpSubtitle->setPlayer(mpPlayer);

    //mpPlayer->setAudioOutput(AudioOutputFactory::create(AudioOutputId_OpenAL));

    EventFilter* const ef = new EventFilter(mpPlayer);
    qApp->installEventFilter(ef);

    connect(ef, SIGNAL(showNextOSD()),
            this, SLOT(showNextOSD()));

    onCaptureConfigChanged();
    onAVFilterVideoConfigChanged();
    onAVFilterAudioConfigChanged();

    connect(&ConfigManager::instance(), SIGNAL(forceFrameRateChanged()),
            this, SLOT(setFrameRate()));

    connect(&ConfigManager::instance(), SIGNAL(captureDirChanged(QString)),
            this, SLOT(onCaptureConfigChanged()));

    connect(&ConfigManager::instance(), SIGNAL(captureFormatChanged(QString)),
            this, SLOT(onCaptureConfigChanged()));

    connect(&ConfigManager::instance(), SIGNAL(captureQualityChanged(int)),
            this, SLOT(onCaptureConfigChanged()));

    connect(&ConfigManager::instance(), SIGNAL(avfilterVideoChanged()),
            this, SLOT(onAVFilterVideoConfigChanged()));

    connect(&ConfigManager::instance(), SIGNAL(avfilterAudioChanged()),
            this, SLOT(onAVFilterAudioConfigChanged()));

    connect(&ConfigManager::instance(), SIGNAL(bufferValueChanged()),
            this, SLOT(onBufferValueChanged()));

    connect(&ConfigManager::instance(), SIGNAL(abortOnTimeoutChanged()),
            this, SLOT(onAbortOnTimeoutChanged()));

    connect(mpStopBtn, SIGNAL(clicked()),
            this, SLOT(stopUnload()));

    connect(mpForwardBtn, SIGNAL(clicked()),
             mpPlayer, SLOT(seekForward()));

    connect(mpBackwardBtn, SIGNAL(clicked()),
            mpPlayer, SLOT(seekBackward()));

    connect(mpVolumeBtn, SIGNAL(clicked()),
            this, SLOT(showHideVolumeBar()));

    connect(mpVolumeSlider, SIGNAL(sliderPressed()),
            this, SLOT(setVolume()));

    connect(mpVolumeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setVolume()));

    connect(mpPlayer, SIGNAL(seekFinished(qint64)),
            this, SLOT(onSeekFinished(qint64)));

    connect(mpPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),
            this, SLOT(onMediaStatusChanged()));

    connect(mpPlayer, SIGNAL(bufferProgressChanged(qreal)),
            this, SLOT(onBufferProgress(qreal)));

    connect(mpPlayer, SIGNAL(error(QtAV::AVError)),
            this, SLOT(handleError(QtAV::AVError)));

    connect(mpPlayer, SIGNAL(started()),
            this, SLOT(onStartPlay()));

    connect(mpPlayer, SIGNAL(stopped()),
            this, SLOT(onStopPlay()));

    connect(mpPlayer, SIGNAL(paused(bool)),
            this, SLOT(onPaused(bool)));

    connect(mpPlayer, SIGNAL(speedChanged(qreal)),
            this, SLOT(onSpeedChange(qreal)));

    connect(mpPlayer, SIGNAL(positionChanged(qint64)),
            this, SLOT(onPositionChange(qint64)));
/*
    connect(mpPlayer, SIGNAL(volumeChanged(qreal)),
            this, SLOT(syncVolumeUi(qreal)));
*/
    connect(mpVideoEQ, SIGNAL(brightnessChanged(int)),
            this, SLOT(onBrightnessChanged(int)));

    connect(mpVideoEQ, SIGNAL(contrastChanged(int)),
            this, SLOT(onContrastChanged(int)));

    connect(mpVideoEQ, SIGNAL(hueChanegd(int)),
            this, SLOT(onHueChanged(int)));

    connect(mpVideoEQ, SIGNAL(saturationChanged(int)),
            this, SLOT(onSaturationChanged(int)));

    connect(mpCaptureBtn, SIGNAL(clicked()),
            mpPlayer->videoCapture(), SLOT(capture()));

    emit ready(); // emit this signal after connection. otherwise the Q_SLOTS may not be called for the first time
}

void MainWindow::onSeekFinished(qint64 pos)
{
    qCDebug(DIGIKAM_AVPLAYER_LOG) << "seek finished at"
                                  << pos
                                  << "/"
                                  << mpPlayer->position();
}

void MainWindow::stopUnload()
{
    mpPlayer->stop();
}

void MainWindow::setupUi()
{
    QVBoxLayout* const mainLayout   = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(QMargins());
    setLayout(mainLayout);

    mpPlayerLayout                  = new QVBoxLayout();
    mpControl                       = new QWidget(this);
    mpControl->setMaximumHeight(30);

    //mpPreview = new QLable(this);

    mpTimeSlider                    = new Slider(mpControl);
    mpTimeSlider->setDisabled(true);
    mpTimeSlider->setTracking(true);
    mpTimeSlider->setOrientation(Qt::Horizontal);
    mpTimeSlider->setMinimum(0);
    mpCurrent                       = new QLabel(mpControl);
    mpCurrent->setToolTip(i18nc("@info", "Current time"));
    mpCurrent->setContentsMargins(QMargins(2, 2, 2, 2));
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    mpEnd                           = new QLabel(mpControl);
    mpEnd->setToolTip(i18nc("@info", "Duration"));
    mpEnd->setContentsMargins(QMargins(2, 2, 2, 2));
    mpEnd->setText(QString::fromLatin1("00:00:00"));
    mpTitle                         = new QLabel(mpControl);
    mpTitle->setToolTip(i18nc("@info", "Render engine"));
    mpTitle->setText(QString::fromLatin1("QPainter"));
    mpTitle->setIndent(8);
    mpSpeed                         = new QLabel(QString::fromLatin1("1.00"));
    mpSpeed->setContentsMargins(QMargins(1, 1, 1, 1));
    mpSpeed->setToolTip(i18nc("@info", "Speed. Ctrl+Up/Down"));

    mpPlayPauseBtn                  = new QToolButton(mpControl);
    mpPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    mpStopBtn                       = new QToolButton(mpControl);
    mpStopBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-stop")));
    mpBackwardBtn                   = new QToolButton(mpControl);
    mpBackwardBtn->setIcon(QIcon::fromTheme(QLatin1String("media-skip-backward")));
    mpForwardBtn                    = new QToolButton(mpControl);
    mpForwardBtn->setIcon(QIcon::fromTheme(QLatin1String("media-skip-forward")));
    mpOpenBtn                       = new QToolButton(mpControl);
    mpOpenBtn->setToolTip(i18nc("@info: open new media", "Open"));
    mpOpenBtn->setIcon(QIcon::fromTheme(QLatin1String("media-eject")));

    mpInfoBtn                       = new QToolButton();
    mpInfoBtn->setToolTip(QString::fromLatin1("Media information"));
    mpInfoBtn->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    mpCaptureBtn                    = new QToolButton();
    mpCaptureBtn->setToolTip(i18nc("@info: button", "Capture"));
    mpCaptureBtn->setIcon(QIcon::fromTheme(QLatin1String("media-record")));
    mpVolumeBtn                     = new QToolButton();
    mpVolumeBtn->setToolTip(i18nc("@info: button", "Volume"));
    mpVolumeBtn->setIcon(QIcon::fromTheme(QLatin1String("player-volume")));

    mpVolumeSlider                  = new Slider();
    mpVolumeSlider->hide();
    mpVolumeSlider->setOrientation(Qt::Horizontal);
    mpVolumeSlider->setMinimum(0);
    const int kVolumeSliderMax      = 100;
    mpVolumeSlider->setMaximum(kVolumeSliderMax);

    //mpVolumeSlider->setMaximumHeight(12);

    mpVolumeSlider->setMaximumWidth(88);
    mpVolumeSlider->setValue(int(1.0 / kVolumeInterval * qreal(kVolumeSliderMax) / 100.0));
    setVolume();

    mpMenuBtn = new QToolButton();
    mpMenuBtn->setIcon(QIcon::fromTheme(QLatin1String("application-menu")));
    mpMenuBtn->setAutoRaise(true);
    mpMenuBtn->setPopupMode(QToolButton::InstantPopup);

    QMenu* subMenu                  = nullptr;
    QWidgetAction* pWA              = nullptr;
    mpMenu                          = new QMenu(mpMenuBtn);
    mpMenu->addAction(i18nc("@action", "Open Url"), this, SLOT(openUrl()));
    mpMenu->addSeparator();

    subMenu                         = new QMenu(i18nc("@option", "Play list"));
    mpMenu->addMenu(subMenu);
    mpPlayList                      = new PlayList(this);
    mpPlayList->setSaveFile(ConfigManager::instance().defaultDir() + QString::fromLatin1("/playlist.qds"));
    mpPlayList->load();

    connect(mpPlayList, SIGNAL(aboutToPlay(QString)),
            this, SLOT(play(QString)));

    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(mpPlayList);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    subMenu                         = new QMenu(i18nc("@option", "History"));
    mpMenu->addMenu(subMenu);
    mpHistory                       = new PlayList(this);
    mpHistory->setMaxRows(20);
    mpHistory->setSaveFile(ConfigManager::instance().defaultDir() + QString::fromLatin1("/history.qds"));
    mpHistory->load();

    connect(mpHistory, SIGNAL(aboutToPlay(QString)),
            this, SLOT(play(QString)));

    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpHistory);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    mpMenu->addSeparator();

    //mpMenu->addAction(i18n("Report"))->setEnabled(false); //report bug, suggestions etc. using maillist?

    mpMenu->addAction(i18nc("@action", "About"), this, SLOT(about()));
    mpMenu->addAction(i18nc("@action", "Setup"), this, SLOT(setup()));
    mpMenu->addSeparator();
    mpMenuBtn->setMenu(mpMenu);
    mpMenu->addSeparator();

    subMenu                         = new QMenu(i18nc("@option", "Speed"));
    mpMenu->addMenu(subMenu);
    QDoubleSpinBox* const pSpeedBox = new QDoubleSpinBox(0);
    pSpeedBox->setRange(0.01, 20);
    pSpeedBox->setValue(1.0);
    pSpeedBox->setSingleStep(0.01);
    pSpeedBox->setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(pSpeedBox);
    subMenu->addAction(pWA); // must add action after the widget action is ready. is it a Qt bug?

    subMenu                         = new ClickableMenu(i18nc("@option", "Repeat"));
    mpMenu->addMenu(subMenu);

    //subMenu->setEnabled(false);

    mpRepeatEnableAction            = subMenu->addAction(i18nc("@action", "Enable"));
    mpRepeatEnableAction->setCheckable(true);

    connect(mpRepeatEnableAction, SIGNAL(toggled(bool)),
            this, SLOT(toggleRepeat(bool)));

    // TODO: move to a func or class

    mpRepeatBox                     = new QSpinBox(0);
    mpRepeatBox->setMinimum(-1);
    mpRepeatBox->setValue(-1);
    mpRepeatBox->setToolTip(QString::fromLatin1("-1: ") + i18nc("@info", "infinity"));
    connect(mpRepeatBox, SIGNAL(valueChanged(int)), SLOT(setRepeateMax(int)));
    QLabel* pRepeatLabel            = new QLabel(i18nc("@label", "Times"));
    QHBoxLayout* hb                 = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatBox);
    QVBoxLayout* const vb           = new QVBoxLayout;
    vb->addLayout(hb);
    pRepeatLabel                    = new QLabel(i18nc("@label: from time-stamp", "From"));
    mpRepeatA                       = new QTimeEdit();
    mpRepeatA->setDisplayFormat(QString::fromLatin1("HH:mm:ss"));
    mpRepeatA->setToolTip(i18nc("@info", "negative value means from the end"));

    connect(mpRepeatA, SIGNAL(timeChanged(QTime)),
            this, SLOT(repeatAChanged(QTime)));

    hb                              = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatA);
    vb->addLayout(hb);
    pRepeatLabel                    = new QLabel(i18nc("@label: to time-stamp", "To"));
    mpRepeatB                       = new QTimeEdit();
    mpRepeatB->setDisplayFormat(QString::fromLatin1("HH:mm:ss"));
    mpRepeatB->setToolTip(i18nc("@info", "negative value means from the end"));

    connect(mpRepeatB, SIGNAL(timeChanged(QTime)),
            this, SLOT(repeatBChanged(QTime)));

    hb                              = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatB);
    vb->addLayout(hb);
    QWidget* wgt                    = new QWidget;
    wgt->setLayout(vb);

    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    pWA->defaultWidget()->setEnabled(false);
    subMenu->addAction(pWA);                    // must add action after the widget action is ready. is it a Qt bug?
    mpRepeatAction                  = pWA;

    mpMenu->addSeparator();

    subMenu                         = new ClickableMenu(i18nc("@action", "Clock"));
    mpMenu->addMenu(subMenu);
    QActionGroup* ag                = new QActionGroup(subMenu);
    ag->setExclusive(true);

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(changeClockType(QAction*)));

    subMenu->addAction(i18nc("@option: clock sync", "Auto"))->setData(-1);
    subMenu->addAction(i18nc("@option: clock sync", "Audio"))->setData(AVClock::AudioClock);
    subMenu->addAction(i18nc("@option: clock sync", "Video"))->setData(AVClock::VideoClock);

    foreach (QAction* const action, subMenu->actions())
    {
        action->setActionGroup(ag);
        action->setCheckable(true);
    }

    QAction* const autoClockAction  = subMenu->actions().at(0);
    autoClockAction->setChecked(true);
    autoClockAction->setToolTip(i18nc("@info", "Take effect in next playback"));

    subMenu                         = new ClickableMenu(i18nc("@option", "Subtitle"));
    mpMenu->addMenu(subMenu);
    QAction* act                    = subMenu->addAction(i18nc("@action", "Enable"));
    act->setCheckable(true);
    act->setChecked(mpSubtitle->isEnabled());

    connect(act, SIGNAL(toggled(bool)),
            this, SLOT(toggoleSubtitleEnabled(bool)));

    act                             = subMenu->addAction(i18nc("@action", "Auto load"));
    act->setCheckable(true);
    act->setChecked(mpSubtitle->autoLoad());

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

    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
    box->setToolTip(i18nc("@info", "FFmpeg supports more subtitles but only render plain text\nLibASS supports 'ass' styles"));

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

    foreach (const QByteArray& cs, QTextCodec::availableCodecs())
    {
        box->addItem(QString::fromLatin1(cs), QString::fromLatin1(cs));
    }

    connect(box, SIGNAL(activated(QString)),
            this, SLOT(setSubtitleCharset(QString)));

    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
    box->setToolTip(i18nc("@info","Auto detect requires libchardet"));

    subMenu                         = new ClickableMenu(i18nc("@option", "Audio track"));
    mpMenu->addMenu(subMenu);
    mpAudioTrackMenu                = subMenu;

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(changeAudioTrack(QAction*)));

    initAudioTrackMenu();

    subMenu = new ClickableMenu(i18nc("@option", "Channel"));
    mpMenu->addMenu(subMenu);
    mpChannelMenu = subMenu;

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(changeChannel(QAction*)));

    subMenu->addAction(i18nc("@action: channel option", "As input"))->setData(AudioFormat::ChannelLayout_Unsupported); // will set to input in resampler if not supported.
    subMenu->addAction(i18nc("@action: channel option", "Stereo"))->setData(AudioFormat::ChannelLayout_Stereo);
    subMenu->addAction(i18nc("@action: channel option", "Mono (center)"))->setData(AudioFormat::ChannelLayout_Center);
    subMenu->addAction(i18nc("@action: channel option", "Left"))->setData(AudioFormat::ChannelLayout_Left);
    subMenu->addAction(i18nc("@action: channel option", "Right"))->setData(AudioFormat::ChannelLayout_Right);
    ag                              = new QActionGroup(subMenu);
    ag->setExclusive(true);

    foreach (QAction* const action, subMenu->actions())
    {
        ag->addAction(action);
        action->setCheckable(true);
    }

    subMenu                         = new QMenu(i18nc("@option", "Aspect ratio"), mpMenu);
    mpMenu->addMenu(subMenu);

    connect(subMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(switchAspectRatio(QAction*)));

    mpARAction                      = subMenu->addAction(i18nc("@action: aspect ratio", "Video"));
    mpARAction->setData(0);
    subMenu->addAction(i18nc("@action: video ratio", "Window"))->setData(-1);
    subMenu->addAction(QString::fromLatin1("4:3"))->setData(4.0   / 3.0);
    subMenu->addAction(QString::fromLatin1("16:9"))->setData(16.0 / 9.0);
    subMenu->addAction(i18nc("@action: video ratio", "Custom"))->setData(-2);

    foreach (QAction* const action, subMenu->actions())
    {
        action->setCheckable(true);
    }

    mpARAction->setChecked(true);

    subMenu                         = new ClickableMenu(i18nc("@option", "Color space"));
    mpMenu->addMenu(subMenu);
    mpVideoEQ                       = new VideoEQConfigPage();

    connect(mpVideoEQ, SIGNAL(engineChanged()),
            this, SLOT(onVideoEQEngineChanged()));

    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(mpVideoEQ);
    subMenu->addAction(pWA);

    subMenu                         = new ClickableMenu(i18nc("@option", "Decoder"));
    mpMenu->addMenu(subMenu);
    mpDecoderConfigPage             = new DecoderConfigPage();
    pWA                             = new QWidgetAction(0);
    pWA->setDefaultWidget(mpDecoderConfigPage);
    subMenu->addAction(pWA);

    subMenu                         = new ClickableMenu(i18nc("@option", "Renderer"));
    mpMenu->addMenu(subMenu);

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

        if (!mpVOAction)
            mpVOAction = voa;
    }

    mpVOAction->setChecked(true);
    mVOActions                       = subMenu->actions();

    mainLayout->addLayout(mpPlayerLayout);
    mainLayout->addWidget(mpTimeSlider);
    mainLayout->addWidget(mpControl);

    QHBoxLayout* const controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(0);
    controlLayout->setContentsMargins(QMargins(1, 1, 1, 1));
    mpControl->setLayout(controlLayout);
    controlLayout->addWidget(mpCurrent);
    controlLayout->addWidget(mpTitle);
    QSpacerItem* const space         = new QSpacerItem(mpPlayPauseBtn->width(), mpPlayPauseBtn->height(), QSizePolicy::MinimumExpanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpVolumeSlider);
    controlLayout->addWidget(mpVolumeBtn);
    controlLayout->addWidget(mpCaptureBtn);
    controlLayout->addWidget(mpPlayPauseBtn);
    controlLayout->addWidget(mpStopBtn);
    controlLayout->addWidget(mpBackwardBtn);
    controlLayout->addWidget(mpForwardBtn);
    controlLayout->addWidget(mpOpenBtn);
    controlLayout->addWidget(mpInfoBtn);
    controlLayout->addWidget(mpSpeed);

    //controlLayout->addWidget(mpSetupBtn);

    controlLayout->addWidget(mpMenuBtn);
    controlLayout->addWidget(mpEnd);

    connect(pSpeedBox, SIGNAL(valueChanged(double)),
            this, SLOT(onSpinBoxChanged(double)));

    connect(mpOpenBtn, SIGNAL(clicked()),
            this, SLOT(openFile()));

    connect(mpPlayPauseBtn, SIGNAL(clicked()),
            this, SLOT(togglePlayPause()));

    connect(mpInfoBtn, SIGNAL(clicked()),
            this, SLOT(showInfo()));

    // valueChanged can be triggered by non-mouse event

    connect(mpTimeSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(seek(int)));

    connect(mpTimeSlider, SIGNAL(sliderPressed()),
            this, SLOT(seek()));

    connect(mpTimeSlider, SIGNAL(onLeave()),
            this, SLOT(onTimeSliderLeave()));

    connect(mpTimeSlider, SIGNAL(onHover(int,int)),
            this, SLOT(onTimeSliderHover(int,int)));

    connect(&ConfigManager::instance(), SIGNAL(userShaderEnabledChanged()),
            this, SLOT(onUserShaderChanged()));

    connect(&ConfigManager::instance(), SIGNAL(intermediateFBOChanged()),
            this, SLOT(onUserShaderChanged()));

    connect(&ConfigManager::instance(), SIGNAL(fragHeaderChanged()),
            this, SLOT(onUserShaderChanged()));

    connect(&ConfigManager::instance(), SIGNAL(fragSampleChanged()),
            this, SLOT(onUserShaderChanged()));

    connect(&ConfigManager::instance(), SIGNAL(fragPostProcessChanged()),
            this, SLOT(onUserShaderChanged()));

    QTimer::singleShot(0, this, SLOT(initPlayer()));
}

void MainWindow::changeChannel(QAction* action)
{
    if (action == mpChannelAction)
    {
        action->toggle();

        return;
    }

    AudioFormat::ChannelLayout cl = (AudioFormat::ChannelLayout)action->data().toInt();
    AudioOutput* const ao         = mpPlayer ? mpPlayer->audio() : 0;                   // getAO() ?

    if (!ao)
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("No audio output!");

        return;
    }

    mpChannelAction->setChecked(false);
    mpChannelAction = action;
    mpChannelAction->setChecked(true);

    if (!ao->close())
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("close audio failed");

        return;
    }

    AudioFormat af(ao->audioFormat());
    af.setChannelLayout(cl);
    ao->setAudioFormat(af);

    if (!ao->open())
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("open audio failed");

        return;
    }
}

void MainWindow::changeAudioTrack(QAction* action)
{
    int track = action->data().toInt();

    if ((mpAudioTrackAction == action) && (track >= 0))
    {
        // external action is always clickable

        action->toggle();

        return;
    }

    if (track < 0)
    {
        QString f = QFileDialog::getOpenFileName(nullptr, i18nc("@title", "Open an external audio track"));

        if (f.isEmpty())
        {
            action->toggle();

            return;
        }

        mpPlayer->setExternalAudio(f);
    }
    else
    {
        mpPlayer->setExternalAudio(QString());

        if (!mpPlayer->setAudioStream(track))
        {
            action->toggle();

            return;
        }
    }

    if (mpAudioTrackAction)
        mpAudioTrackAction->setChecked(false);

    mpAudioTrackAction = action;
    mpAudioTrackAction->setChecked(true);

    if (mpStatisticsView && mpStatisticsView->isVisible())
        mpStatisticsView->setStatistics(mpPlayer->statistics());
}

void MainWindow::changeVO(QAction* action)
{
    if (action == mpVOAction)
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
                              i18nc("@info", "not availabe on your platform!"));

        return;
    }
}

void MainWindow::processPendingActions()
{
    if (mHasPendingPlay)
    {
        mHasPendingPlay = false;
        play(mFile);
    }
}

void MainWindow::setAudioBackends(const QStringList& backends)
{
    mAudioBackends = backends;
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

    mpOSD->uninstall();
    mpSubtitle->uninstall();
    renderer->widget()->setMouseTracking(true); // mouseMoveEvent without press.
    mpPlayer->setRenderer(renderer);
    QWidget* r = nullptr;

    if (mpRenderer)
        r = mpRenderer->widget();

    // release old renderer and add new

    if (r)
    {
        mpPlayerLayout->removeWidget(r);

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

    mpRenderer = renderer;

    // setInSize ?

    mpPlayerLayout->addWidget(renderer->widget());

    if (mpVOAction)
    {
        mpVOAction->setChecked(false);
    }

    foreach (QAction* const action, mVOActions)
    {
        if (action->data() == renderer->id())
        {
            mpVOAction = action;

            break;
        }
    }

    if (mpVOAction)
    {
        mpVOAction->setChecked(true);
    }

    mpTitle->setText(mpVOAction->text());
    const VideoRendererId vid = mpPlayer->renderer()->id();

    if      (   vid == VideoRendererId_GLWidget
             || vid == VideoRendererId_GLWidget2
             || vid == VideoRendererId_OpenGLWidget
       )
    {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale
                                                                   << VideoEQConfigPage::GLSL);
        mpVideoEQ->setEngine(VideoEQConfigPage::GLSL);
        mpPlayer->renderer()->forcePreferredPixelFormat(true);
    }
    else if (vid == VideoRendererId_XV)
    {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::XV);
        mpVideoEQ->setEngine(VideoEQConfigPage::XV);
        mpPlayer->renderer()->forcePreferredPixelFormat(true);
    }
    else
    {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale);
        mpVideoEQ->setEngine(VideoEQConfigPage::SWScale);
        mpPlayer->renderer()->forcePreferredPixelFormat(false);
    }

    onVideoEQEngineChanged();
    mpOSD->installTo(mpRenderer);
    mpSubtitle->installTo(mpRenderer);
    onUserShaderChanged();

#define GL_ASS 0
#if GL_ASS

    GLSLFilter* const glsl = new GLSLFilter(this);
    glsl->setOutputSize(QSize(4096, 2160));

    //mpRenderer->installFilter(glsl);

    if (mpRenderer->opengl())
    {
        connect(mpRenderer->opengl(), &OpenGLVideo::beforeRendering,
                [this]()
            {
                OpenGLVideo* const glv = mpRenderer->opengl();
                glv->setSubImages(mpSubtitle->subImages(glv->frameTime(), glv->frameWidth(), glv->frameHeight()));
            }
        );
    }

#endif

    return true;
}

void MainWindow::play(const QString& name)
{
    mFile = name;

    if (!mIsReady)
    {
        mHasPendingPlay = true;

        return;
    }

    mTitle = mFile;

    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://")))
    {
        mTitle = QFileInfo(mFile).fileName();
    }

    setWindowTitle(mTitle);
    mpPlayer->stop();       // if no stop, in setPriority decoder will reopen
    mpPlayer->setFrameRate(ConfigManager::instance().forceFrameRate());

    if (!mAudioBackends.isEmpty())
        mpPlayer->audio()->setBackends(mAudioBackends);

    if (!mpRepeatEnableAction->isChecked())
        mRepeateMax = 0;

    mpPlayer->setInterruptOnTimeout(ConfigManager::instance().abortOnTimeout());
    mpPlayer->setInterruptTimeout(ConfigManager::instance().timeout()*1000.0);
    mpPlayer->setBufferMode(QtAV::BufferPackets);
    mpPlayer->setBufferValue(ConfigManager::instance().bufferValue());
    mpPlayer->setRepeat(mRepeateMax);
    mpPlayer->setPriority(idsFromNames(ConfigManager::instance().decoderPriorityNames()));
    mpPlayer->setOptionsForAudioCodec(mpDecoderConfigPage->audioDecoderOptions());
    mpPlayer->setOptionsForVideoCodec(mpDecoderConfigPage->videoDecoderOptions());

    if (ConfigManager::instance().avformatOptionsEnabled())
        mpPlayer->setOptionsForFormat(ConfigManager::instance().avformatOptions());

    qCDebug(DIGIKAM_AVPLAYER_LOG) << ConfigManager::instance().avformatOptions();

    PlayListItem item;
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setLastTime(0);
    mpHistory->remove(mFile);
    mpHistory->insertItemAt(item, 0);
    mpPlayer->play(name);
}

void MainWindow::play(const QUrl& url)
{
    play(QUrl::fromPercentEncoding(url.toEncoded()));
}

void MainWindow::setVideoDecoderNames(const QStringList& vd)
{
    QStringList vdnames;

    foreach (const QString& v, vd)
    {
        vdnames << v.toLower();
    }

    QStringList vidp;
    QStringList vids = idsToNames(VideoDecoder::registered());

    foreach (const QString& v, vids)
    {
        if (vdnames.contains(v.toLower()))
        {
            vidp.append(v);
        }
    }

    ConfigManager::instance().setDecoderPriorityNames(vidp);
}

void MainWindow::openFile()
{
    QString file = QFileDialog::getOpenFileName(nullptr, i18nc("@title", "Open a media file"),
                                                ConfigManager::instance().lastFile());

    if (file.isEmpty())
        return;

    ConfigManager::instance().setLastFile(file);
    play(file);
}

void MainWindow::togglePlayPause()
{
    if (mpPlayer->isPlaying())
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("isPaused = %d", mpPlayer->isPaused());
        mpPlayer->pause(!mpPlayer->isPaused());
    }
    else
    {
        if (mFile.isEmpty())
            return;

        if (!mpPlayer->isPlaying())
            play(mFile);
        else
            mpPlayer->play();

        mpPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("player-volume")));
    }
}

void MainWindow::showNextOSD()
{
    if (!mpOSD)
        return;

    mpOSD->useNextShowType();
}

void MainWindow::onSpinBoxChanged(double v)
{
    if (!mpPlayer)
        return;

    mpPlayer->setSpeed(v);
}

void MainWindow::onPaused(bool p)
{
    if (p)
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("start pausing...");
        mpPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    }
    else
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("stop pausing...");
        mpPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
    }
}

void MainWindow::onStartPlay()
{
    mpRenderer->setRegionOfInterest(QRectF());
    mFile  = mpPlayer->file();       // open from EventFilter's menu
    mTitle = mFile;

    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://")))
        mTitle = QFileInfo(mFile).fileName();

    setWindowTitle(mTitle);

    mpPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
    mpTimeSlider->setMinimum(mpPlayer->mediaStartPosition());
    mpTimeSlider->setMaximum(mpPlayer->mediaStopPosition());
    mpTimeSlider->setValue(0);
    mpTimeSlider->setEnabled(mpPlayer->isSeekable());
    mpEnd->setText(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()).toString(QString::fromLatin1("HH:mm:ss")));
    setVolume();
    mShowControl = 0;

    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));

    ScreenSaver::instance().disable();
    initAudioTrackMenu();
    mpRepeatA->setMinimumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
    mpRepeatA->setMaximumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
    mpRepeatB->setMinimumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
    mpRepeatB->setMaximumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
    mpRepeatA->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->startPosition()));
    mpRepeatB->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->stopPosition()));
    mCursorTimer      = startTimer(3000);
    PlayListItem item = mpHistory->itemAt(0);
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setDuration(mpPlayer->duration());
    mpHistory->setItemAt(item, 0);
    updateChannelMenu();

    if (mpStatisticsView && mpStatisticsView->isVisible())
        mpStatisticsView->setStatistics(mpPlayer->statistics());
}

void MainWindow::onStopPlay()
{
    mpPlayer->setPriority(idsFromNames(ConfigManager::instance().decoderPriorityNames()));

    if ((mpPlayer->currentRepeat() >= 0) && (mpPlayer->currentRepeat() < mpPlayer->repeat()))
        return;

    // use shortcut to replay in EventFilter, the options will not be set, so set here

    mpPlayer->setFrameRate(ConfigManager::instance().forceFrameRate());
    mpPlayer->setOptionsForAudioCodec(mpDecoderConfigPage->audioDecoderOptions());
    mpPlayer->setOptionsForVideoCodec(mpDecoderConfigPage->videoDecoderOptions());

    if (ConfigManager::instance().avformatOptionsEnabled())
        mpPlayer->setOptionsForFormat(ConfigManager::instance().avformatOptions());

    mpPlayPauseBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    mpTimeSlider->setValue(0);

    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("disable slider");

    mpTimeSlider->setDisabled(true);
    mpTimeSlider->setMinimum(0);
    mpTimeSlider->setMaximum(0);
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    mpEnd->setText(QString::fromLatin1("00:00:00"));
    tryShowControlBar();
    ScreenSaver::instance().enable();
    toggleRepeat(false);

    //mRepeateMax = 0;

    killTimer(mCursorTimer);
    unsetCursor();

    if (m_preview)
        m_preview->setFile(QString());
}

void MainWindow::onSpeedChange(qreal speed)
{
    mpSpeed->setText(QString::fromLatin1("%1").arg(speed, 4, 'f', 2, QLatin1Char('0')));
}

void MainWindow::setFrameRate()
{
    if (!mpPlayer)
        return;

    mpPlayer->setFrameRate(ConfigManager::instance().forceFrameRate());
}

void MainWindow::seek(int value)
{
    mpPlayer->setSeekType(AccurateSeek);
    mpPlayer->seek((qint64)value);

    if (!m_preview || !ConfigManager::instance().previewEnabled())
        return;

    m_preview->setTimestamp(value);
    m_preview->preview();
    m_preview->setWindowFlags(m_preview->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_preview->resize(ConfigManager::instance().previewWidth(), ConfigManager::instance().previewHeight());
    m_preview->show();
}

void MainWindow::seek()
{
    seek(mpTimeSlider->value());
}

void MainWindow::showHideVolumeBar()
{
    if (mpVolumeSlider->isHidden())
    {
        mpVolumeSlider->show();
    }
    else
    {
        mpVolumeSlider->hide();
    }
}

void MainWindow::setVolume()
{
    AudioOutput* const ao = mpPlayer ? mpPlayer->audio() : nullptr;
    qreal v               = qreal(mpVolumeSlider->value())*kVolumeInterval;

    if (ao)
    {
        if (qAbs(int(ao->volume()/kVolumeInterval) - mpVolumeSlider->value()) >= int(0.1 / kVolumeInterval))
        {
            ao->setVolume(v);
        }
    }

    mpVolumeSlider->setToolTip(QString::number(v));
    mpVolumeBtn->setToolTip(QString::number(v));
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    Q_UNUSED(e);

    if (mpPlayer)
        mpPlayer->stop();

    qApp->quit();
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    QWidget::resizeEvent(e);
/*
    if (mpTitle)
        QLabelSetElideText(mpTitle, QFileInfo(mFile).fileName(), e->size().width());
*/
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mCursorTimer)
    {
        if (mpControl->isVisible())
            return;

        setCursor(Qt::BlankCursor);
    }
}

void MainWindow::onPositionChange(qint64 pos)
{
    if (mpPlayer->isSeekable())
        mpTimeSlider->setValue(pos);

    mpCurrent->setText(QTime(0, 0, 0).addMSecs(pos).toString(QString::fromLatin1("HH:mm:ss")));

    //setWindowTitle(QString::number(mpPlayer->statistics().video_only.currentDisplayFPS(), 'f', 2).append(" ").append(mTitle));
}

void MainWindow::repeatAChanged(const QTime& t)
{
    if (!mpPlayer)
        return;

    mpPlayer->setStartPosition(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::repeatBChanged(const QTime& t)
{
    if (!mpPlayer)
        return;

    // when this slot is called? even if only range is set?

    if (t <= mpRepeatA->time())
        return;

    mpPlayer->setStopPosition(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    mControlOn = (e->key() == Qt::Key_Control);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e)
{
    Q_UNUSED(e);
    mControlOn = false;
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    if (!mControlOn)
        return;

    mGlobalMouse = e->globalPos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    unsetCursor();

    if (e->pos().y() > (height() - mpTimeSlider->height() - mpControl->height()))
    {
        if (mShowControl == 0)
        {
            mShowControl = 1;
            tryShowControlBar();
        }
    }
    else
    {
        if (mShowControl == 1)
        {
            mShowControl = 0;
            QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
        }
    }

    if (mControlOn && (e->button() == Qt::LeftButton))
    {
        if (!mpRenderer || !mpRenderer->widget())
            return;

        QRectF roi    = mpRenderer->realROI();
        QPointF delta = e->pos() - mGlobalMouse;
        roi.moveLeft(-delta.x());
        roi.moveTop(-delta.y());
        mpRenderer->setRegionOfInterest(roi);
    }
}

void MainWindow::wheelEvent(QWheelEvent* e)
{
    if (!mpRenderer || !mpRenderer->widget())
    {
        return;
    }

    QPoint dp;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    qreal deg = e->angleDelta().y() / 8;
    dp        = e->pixelDelta();

#else

    qreal deg = e->delta() / 8;

#endif // QT_VERSION

#ifdef WHEEL_SPEED

    if (!mControlOn)
    {
        qreal speed = mpPlayer->speed();
        mpPlayer->setSpeed(qMax(0.01, speed + deg / 15.0*0.02));

        return;
    }

#endif // WHEEL_SPEED

    QPointF p  = mpRenderer->widget()->mapFrom(this, e->position().toPoint());
    QPointF fp = mpRenderer->mapToFrame(p);

    //qCDebug(DIGIKAM_AVPLAYER_LOG) <<  p << fp;

    if (fp.x() < 0)
        fp.setX(0);

    if (fp.y() < 0)
        fp.setY(0);

    if (fp.x() > mpRenderer->videoFrameSize().width())
        fp.setX(mpRenderer->videoFrameSize().width());

    if (fp.y() > mpRenderer->videoFrameSize().height())
        fp.setY(mpRenderer->videoFrameSize().height());

    QRectF viewport = QRectF(mpRenderer->mapToFrame(QPointF(0, 0)),
                             mpRenderer->mapToFrame(QPointF(mpRenderer->rendererWidth(),
                             mpRenderer->rendererHeight())));

    Q_UNUSED(viewport);

    //qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("vo: (%.1f, %.1f)=> frame: (%.1f, %.1f)", p.x(), p.y(), fp.x(), fp.y());

    qreal zoom = 1.0 + deg*3.14 / 180.0;

    if (!dp.isNull())
    {
        zoom = 1.0 + (qreal)dp.y() / 100.0;
    }

    static qreal z = 1.0;
    z             *= zoom;

    if (z < 1.0)
        z = 1.0;

    qreal x0 = fp.x() - fp.x() / z;
    qreal y0 = fp.y() - fp.y() / z;

    //qCDebug(DIGIKAM_AVPLAYER_LOG) << "fr: " << QRectF(x0, y0, qreal(mpRenderer->videoFrameSize().width())/z, qreal(mpRenderer->videoFrameSize().height())/z) << fp << z;

    mpRenderer->setRegionOfInterest(QRectF(x0, y0,
                                           qreal(mpRenderer->videoFrameSize().width())  / z,
                                           qreal(mpRenderer->videoFrameSize().height()) / z));

/*
    QTransform m;
    m.translate(fp.x(), fp.y());
    m.scale(1.0/zoom, 1.0/zoom);
    m.translate(-fp.x(), -fp.y());
    QRectF r = m.mapRect(mpRenderer->realROI());
    mpRenderer->setRegionOfInterest((r | m.mapRect(viewport))&QRectF(QPointF(0,0), mpRenderer->videoFrameSize()));
*/
}

void MainWindow::about()
{
    QtAV::about();
}

void MainWindow::openUrl()
{
    QString url = QInputDialog::getText(nullptr, i18nc("@title", "Open an url"), i18nc("@info", "Url"));

    if (url.isEmpty())
        return;

    play(url);
}

void MainWindow::updateChannelMenu()
{
    if (mpChannelAction)
        mpChannelAction->setChecked(false);

    AudioOutput* const ao = mpPlayer ? mpPlayer->audio() : 0; // getAO() ?

    if (!ao)
    {
        return;
    }

    AudioFormat::ChannelLayout cl = ao->audioFormat().channelLayout();
    QList<QAction*> as            = mpChannelMenu->actions();

    foreach (QAction* const action, as)
    {
        if (action->data().toInt() != (int)cl)
            continue;

        action->setChecked(true);
        mpChannelAction = action;

        break;
    }
}

void MainWindow::initAudioTrackMenu()
{
    int track  = -2;
    QAction* a = nullptr;
    QList<QAction*> as;
    int tracks = 0;

    if (!mpPlayer)
    {
        a = mpAudioTrackMenu->addAction(i18nc("@action: audio track", "External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        mpAudioTrackAction = 0;

        goto end;
    }

    track  = mpPlayer->currentAudioStream();
    as     = mpAudioTrackMenu->actions();
    tracks = mpPlayer->audioStreamCount();

    if (mpAudioTrackAction && ((tracks == as.size() - 1)) && (mpAudioTrackAction->data().toInt() == track))
        return;

    while ((tracks + 1) < as.size())
    {
        a = as.takeLast();
        mpAudioTrackMenu->removeAction(a);
        delete a;
    }

    if (as.isEmpty())
    {
        a = mpAudioTrackMenu->addAction(i18nc("@action: audio track", "External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        mpAudioTrackAction = 0;
    }

    while (tracks + 1 > as.size())
    {
        a = mpAudioTrackMenu->addAction(QString::number(as.size()-1));
        a->setData(as.size()-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
    }

end:

    foreach (QAction* const ac, as)
    {
        if ((ac->data().toInt() == track) && (track >= 0))
        {
            if (mpPlayer && mpPlayer->externalAudio().isEmpty())
            {
                qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("track found!");
                mpAudioTrackAction = ac;
                ac->setChecked(true);
            }
        }
        else
        {
            ac->setChecked(false);
        }
    }

    if (mpPlayer && !mpPlayer->externalAudio().isEmpty())
    {
        mpAudioTrackAction = as.first();
    }

    if (mpAudioTrackAction)
        mpAudioTrackAction->setChecked(true);
}

void MainWindow::switchAspectRatio(QAction *action)
{
    qreal r = action->data().toDouble();

    if ((action == mpARAction) && (r != -2))
    {
        action->toggle(); // check state changes if clicked

        return;
    }

    if      (r == 0)
    {
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::VideoAspectRatio);
    }
    else if (r == -1)
    {
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::RendererAspectRatio);
    }
    else
    {
        if (r == -2)
            r = QInputDialog::getDouble(0, i18nc("@option", "Aspect ratio"), QString(), 1.0);

        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::CustomAspectRation);
        mpPlayer->renderer()->setOutAspectRatio(r);
    }

    mpARAction->setChecked(false);
    mpARAction = action;
    mpARAction->setChecked(true);
}

void MainWindow::toggleRepeat(bool r)
{
    mpRepeatEnableAction->setChecked(r);
    mpRepeatAction->defaultWidget()->setEnabled(r); // why need defaultWidget?

    if (r)
    {
        mRepeateMax = mpRepeatBox->value();
    }
    else
    {
        mRepeateMax = 0;
    }

    if (mpPlayer)
    {
        mpPlayer->setRepeat(mRepeateMax);

        if (r)
        {
            repeatAChanged(mpRepeatA->time());
            repeatBChanged(mpRepeatB->time());
        }
        else
        {
            mpPlayer->setTimeRange(0);
        }
    }
}

void MainWindow::setRepeateMax(int m)
{
    mRepeateMax = m;

    if (mpPlayer)
    {
        mpPlayer->setRepeat(m);
    }
}

void MainWindow::playOnlineVideo(QAction* action)
{
    mTitle = action->text();
    play(action->data().toString());
}

void MainWindow::onPlayListClick(const QString& key, const QString& value)
{
    mTitle = key;
    play(value);
}

void MainWindow::tryHideControlBar()
{
    if (mShowControl > 0)
    {
        return;
    }

    if (mpControl->isHidden() && mpTimeSlider->isHidden())
        return;

    mpControl->hide();
    mpTimeSlider->hide();
    workaroundRendererSize();
}

void MainWindow::tryShowControlBar()
{
    unsetCursor();

    if (mpTimeSlider && mpTimeSlider->isHidden())
        mpTimeSlider->show();

    if (mpControl && mpControl->isHidden())
        mpControl->show();
}

void MainWindow::showInfo()
{
    if (!mpStatisticsView)
        mpStatisticsView = new StatisticsView();

    if (mpPlayer)
        mpStatisticsView->setStatistics(mpPlayer->statistics());

    mpStatisticsView->show();
}

void MainWindow::onTimeSliderHover(int pos, int value)
{
    QPoint gpos = mapToGlobal(mpTimeSlider->pos() + QPoint(pos, 0));
    QToolTip::showText(gpos, QTime(0, 0, 0).addMSecs(value).toString(QString::fromLatin1("HH:mm:ss")));

    if (!ConfigManager::instance().previewEnabled())
        return;

    if (!m_preview)
        m_preview = new VideoPreviewWidget();

    m_preview->setFile(mpPlayer->file());
    m_preview->setTimestamp(value);
    m_preview->preview();
    const int w = ConfigManager::instance().previewWidth();
    const int h = ConfigManager::instance().previewHeight();
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

        qCWarning(DIGIKAM_AVPLAYER_LOG) << "invalid sender() " << sender() << player;

        return;
    }

    switch (player->mediaStatus())
    {
        case NoMedia:
            status = i18nc("@info: media loading", "No media");
            break;

        case InvalidMedia:
            status = i18nc("@info: media loading", "Invalid meida");
            break;

        case BufferingMedia:
            status = i18nc("@info: media loading", "Buffering...");
            break;

        case BufferedMedia:
            status = i18nc("@info: media loading", "Buffered");
            break;

        case LoadingMedia:
            status = i18nc("@info: media loading", "Loading...");
            break;

        case LoadedMedia:
            status = i18nc("@info: media loading", "Loaded");
            break;

        case StalledMedia:
            status = i18nc("@info: media loading", "Stalled");
            break;

        default:
            status = QString();
            onStopPlay();
            break;
    }

    qCDebug(DIGIKAM_AVPLAYER_LOG) << "status changed " << status;
    setWindowTitle(status + QString::fromLatin1(" ") + mTitle);
}

void MainWindow::onBufferProgress(qreal percent)
{
    const qreal bs = mpPlayer->bufferSpeed();
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
                   QString::fromLatin1("%1").arg(percent*100.0, 5, 'f', 1), s, mTitle));
}

void MainWindow::onVideoEQEngineChanged()
{
    VideoRenderer* const vo     = mpPlayer->renderer();
    VideoEQConfigPage::Engine e = mpVideoEQ->engine();

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

    onBrightnessChanged(mpVideoEQ->brightness()*100.0);
    onContrastChanged(mpVideoEQ->contrast()*100.0);
    onHueChanged(mpVideoEQ->hue()*100.0);
    onSaturationChanged(mpVideoEQ->saturation()*100.0);
}

void MainWindow::onBrightnessChanged(int b)
{
    VideoRenderer* const vo = mpPlayer->renderer();

    if ((mpVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setBrightness(mpVideoEQ->brightness()))
    {
        mpPlayer->setBrightness(0);
    }
    else
    {
        vo->setBrightness(0);
        mpPlayer->setBrightness(b);
    }
}

void MainWindow::onContrastChanged(int c)
{
    VideoRenderer* const vo = mpPlayer->renderer();

    if ((mpVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setContrast(mpVideoEQ->contrast()))
    {
        mpPlayer->setContrast(0);
    }
    else
    {
        vo->setContrast(0);
        mpPlayer->setContrast(c);
    }
}

void MainWindow::onHueChanged(int h)
{
    VideoRenderer* const vo = mpPlayer->renderer();

    if ((mpVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setHue(mpVideoEQ->hue()))
    {
        mpPlayer->setHue(0);
    }
    else
    {
        vo->setHue(0);
        mpPlayer->setHue(h);
    }
}

void MainWindow::onSaturationChanged(int s)
{
    VideoRenderer* const vo = mpPlayer->renderer();

    if ((mpVideoEQ->engine() != VideoEQConfigPage::SWScale) &&
        vo->setSaturation(mpVideoEQ->saturation()))
    {
        mpPlayer->setSaturation(0);
    }
    else
    {
        vo->setSaturation(0);
        mpPlayer->setSaturation(s);
    }
}

void MainWindow::onCaptureConfigChanged()
{
    mpPlayer->videoCapture()->setCaptureDir(ConfigManager::instance().captureDir());
    mpPlayer->videoCapture()->setQuality(ConfigManager::instance().captureQuality());

    if (ConfigManager::instance().captureFormat().toLower() == QLatin1String("original"))
    {
        mpPlayer->videoCapture()->setOriginalFormat(true);
    }
    else
    {
        mpPlayer->videoCapture()->setOriginalFormat(false);
        mpPlayer->videoCapture()->setSaveFormat(ConfigManager::instance().captureFormat());
    }

    mpCaptureBtn->setToolTip(i18nc("@info", "Capture video frame\nSave to: %1\nFormat: %2",
                             mpPlayer->videoCapture()->captureDir(),
                             ConfigManager::instance().captureFormat()));
}

void MainWindow::onAVFilterVideoConfigChanged()
{
    if (mpVideoFilter)
    {
        mpVideoFilter->uninstall();
        delete mpVideoFilter;
        mpVideoFilter = 0;
    }

    mpVideoFilter = new LibAVFilterVideo(this);
    mpVideoFilter->setEnabled(ConfigManager::instance().avfilterVideoEnable());
    mpPlayer->installFilter(mpVideoFilter);
    mpVideoFilter->setOptions(ConfigManager::instance().avfilterVideoOptions());
}

void MainWindow::onAVFilterAudioConfigChanged()
{
    if (mpAudioFilter)
    {
        mpAudioFilter->uninstall();
        delete mpAudioFilter;
        mpAudioFilter = 0;
    }

    mpAudioFilter = new LibAVFilterAudio(this);
    mpAudioFilter->setEnabled(ConfigManager::instance().avfilterAudioEnable());
    mpAudioFilter->installTo(mpPlayer);
    mpAudioFilter->setOptions(ConfigManager::instance().avfilterAudioOptions());
}

void MainWindow::onBufferValueChanged()
{
    if (!mpPlayer)
        return;

    mpPlayer->setBufferValue(ConfigManager::instance().bufferValue());
}

void MainWindow::onAbortOnTimeoutChanged()
{
    if (!mpPlayer)
        return;

    mpPlayer->setInterruptOnTimeout(ConfigManager::instance().abortOnTimeout());
}

void MainWindow::onUserShaderChanged()
{
    if (!mpRenderer || !mpRenderer->opengl())
        return;

#ifndef QT_NO_OPENGL

    if (ConfigManager::instance().userShaderEnabled())
    {
        if (ConfigManager::instance().intermediateFBO())
        {
            if (!m_glsl)
                m_glsl = new GLSLFilter(this);

            m_glsl->installTo(mpRenderer);
        }
        else
        {
            if (m_glsl)
                m_glsl->uninstall();
        }

        if (!m_shader)
            m_shader = new DynamicShaderObject(this);

        m_shader->setHeader(ConfigManager::instance().fragHeader());
        m_shader->setSample(ConfigManager::instance().fragSample());
        m_shader->setPostProcess(ConfigManager::instance().fragPostProcess());
        mpRenderer->opengl()->setUserShader(m_shader);
    }
    else
    {
        mpRenderer->opengl()->setUserShader(nullptr);
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
    mpSubtitle->setEnabled(value);
}

void MainWindow::toggleSubtitleAutoLoad(bool value)
{
    mpSubtitle->setAutoLoad(value);
}

void MainWindow::openSubtitle()
{
    QString file = QFileDialog::getOpenFileName(nullptr, i18nc("@title", "Open a subtitle file"));

    if (file.isEmpty())
        return;

    mpSubtitle->setFile(file);
}

void MainWindow::setSubtitleCharset(const QString& charSet)
{
    Q_UNUSED(charSet);

    QComboBox* const box = qobject_cast<QComboBox*>(sender());

    if (!box)
        return;

    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
}

void MainWindow::setSubtitleEngine(const QString &value)
{
    Q_UNUSED(value)

    QComboBox* const box = qobject_cast<QComboBox*>(sender());

    if (!box)
        return;

    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
}

void MainWindow::changeClockType(QAction* action)
{
    action->setChecked(true);
    int value = action->data().toInt();

    if (value < 0)
    {
        mpPlayer->masterClock()->setClockAuto(true);

        // TODO: guess clock type

        return;
    }

    mpPlayer->masterClock()->setClockAuto(false);
    mpPlayer->masterClock()->setClockType(AVClock::ClockType(value));
}

void MainWindow::syncVolumeUi(qreal value)
{
    const int v(value / kVolumeInterval);

    if (mpVolumeSlider->value() == v)
        return;

    mpVolumeSlider->setValue(v);
}

void MainWindow::workaroundRendererSize()
{
    if (!mpRenderer)
        return;

    QSize s = rect().size();
/*
    resize(QSize(s.width()-1, s.height()-1));
    resize(s); //window resize to fullscreen size will create another fullScreenChange event
*/
    mpRenderer->widget()->resize(QSize(s.width()+1, s.height()+1));
    mpRenderer->widget()->resize(s);
}

} // namespace AVPlayer
