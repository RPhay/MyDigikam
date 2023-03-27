/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef AV_PLAYER_MAIN_WINDOW_P_H
#define AV_PLAYER_MAIN_WINDOW_P_H

#include "MainWindow.h"

// Qt includes

#include <QLocale>
#include <QTimer>
#include <QTimeEdit>
#include <QLabel>
#include <QApplication>
#include <QStandardPaths>
#include <QActionGroup>
#include <QDesktopServices>
#include <QFileInfo>
#if (QT_VERSION <= QT_VERSION_CHECK(5, 99, 0))
    #include <QTextCodec>
#endif
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

class Q_DECL_HIDDEN MainWindow::Private
{
public:

    Private();
    ~Private();

    void workaroundRendererSize(MainWindow* const parent);
    void QLabelSetElideText(QLabel* const label, const QString& text, int W = 0);

public:

    bool                            IsReady             = false;
    bool                            HasPendingPlay      = false;
    bool                            ControlOn           = false;

    int                             CursorTimer;
    int                             ShowControl         = 2;           ///< 0: can hide, 1: show and playing, 2: always show(not playing)
    int                             RepeateMax          = 0;

    QStringList                     AudioBackends;
    QVBoxLayout*                    pPlayerLayout       = nullptr;

    QWidget*                        pControl            = nullptr;
    QLabel*                         pCurrent            = nullptr;
    QLabel*                         pEnd                = nullptr;
    QLabel*                         pTitle              = nullptr;
    QLabel*                         pSpeed              = nullptr;
    QtAV::AVPlayerSlider*           pTimeSlider         = nullptr;
    QtAV::AVPlayerSlider*           pVolumeSlider       = nullptr;
    QToolButton*                    pVolumeBtn          = nullptr;
    QToolButton*                    pPlayPauseBtn       = nullptr;
    QToolButton*                    pStopBtn            = nullptr;
    QToolButton*                    pForwardBtn         = nullptr;
    QToolButton*                    pBackwardBtn        = nullptr;
    QToolButton*                    pOpenBtn            = nullptr;
    QToolButton*                    pInfoBtn            = nullptr;
    QToolButton*                    pMenuBtn            = nullptr;
    QToolButton*                    pSetupBtn           = nullptr;
    QToolButton*                    pCaptureBtn         = nullptr;
    QMenu*                          pMenu               = nullptr;
    QAction*                        pVOAction           = nullptr;             // remove mpVOAction if vo.id() is supported
    QAction*                        pARAction           = nullptr;
    QAction*                        pRepeatEnableAction = nullptr;
    QWidgetAction*                  pRepeatAction       = nullptr;
    QSpinBox*                       pRepeatBox          = nullptr;
    QTimeEdit*                      pRepeatA            = nullptr;
    QTimeEdit*                      pRepeatB            = nullptr;
    QAction*                        pAudioTrackAction   = nullptr;
    QMenu*                          pAudioTrackMenu     = nullptr;
    QMenu*                          pChannelMenu        = nullptr;
    QAction*                        pChannelAction      = nullptr;
    QList<QAction*>                 VOActions;

    QtAV::AVClock*                  pClock              = nullptr;
    QtAV::AVPlayerCore*             pPlayer             = nullptr;
    QtAV::VideoRenderer*            pRenderer           = nullptr;
    QtAV::LibAVFilterVideo*         pVideoFilter        = nullptr;
    QtAV::LibAVFilterAudio*         pAudioFilter        = nullptr;

    QString                         File;
    QString                         Title;
    QLabel*                         pPreview            = nullptr;

    QtAV::DecoderConfigPage*        pDecoderConfigPage  = nullptr;
    QtAV::VideoEQConfigPage*        pVideoEQ            = nullptr;

    PlayList*                       pPlayList           = nullptr;
    PlayList*                       pHistory            = nullptr;

    QPointF                         GlobalMouse;
    StatisticsView*                 pStatisticsView     = nullptr;

    OSDFilter*                      pOSD                = nullptr;
    QtAV::SubtitleFilter*           pSubtitle           = nullptr;
    QtAV::VideoPreviewWidget*       preview             = nullptr;
    QtAV::DynamicShaderObject*      shader              = nullptr;
    QtAV::GLSLFilter*               glsl                = nullptr;

    const qreal                     kVolumeInterval     = 0.04;
};

} // namespace AVPlayer

#endif // AV_PLAYER_MAIN_WINDOW_P_H
