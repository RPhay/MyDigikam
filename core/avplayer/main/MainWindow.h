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

#ifndef AV_PLAYER_MAIN_WINDOW_H
#define AV_PLAYER_MAIN_WINDOW_H

// Qt includes

#include <QWidget>
#include <QUrl>

class QWidgetAction;
class QToolButton;
class QMenu;
class QTimeEdit;
class QVBoxLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QTimeEdit;

namespace QtAV
{
class AVPlayerSlider;
class AudioOutput;
class AVError;
class AVPlayerCore;
class AVClock;
class VideoRenderer;
class LibAVFilterAudio;
class LibAVFilterVideo;
class SubtitleFilter;
class VideoPreviewWidget;
class DynamicShaderObject;
class GLSLFilter;
class DecoderConfigPage;
class VideoEQConfigPage;
}

namespace AVPlayer
{

class Button;
class PlayList;
class StatisticsView;
class OSDFilter;
class AVFilterSubtitle;
class Preview;

class MainWindow : public QWidget
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget* const parent = nullptr);
    ~MainWindow();

    void setAudioBackends(const QStringList& backends);
    bool setRenderer(QtAV::VideoRenderer* const renderer);
    void setVideoDecoderNames(const QStringList& vd);

public Q_SLOTS:

    void play(const QString& name);
    void play(const QUrl& url);
    void openFile();
    void togglePlayPause();
    void showNextOSD();

Q_SIGNALS:

    void ready();

private Q_SLOTS:

    void stopUnload();
    void about();
    void openUrl();
    void initAudioTrackMenu();
    void updateChannelMenu();
    void switchAspectRatio(QAction* action);
    void toggleRepeat(bool);
    void setRepeateMax(int);
    void changeVO(QAction* action);
    void changeChannel(QAction* action);
    void changeAudioTrack(QAction* action);
    void playOnlineVideo(QAction* action);
    void onPlayListClick(const QString& key, const QString& value);
    void processPendingActions();
    void initPlayer();
    void setupUi();
    void onSpinBoxChanged(double v);
    void onStartPlay();
    void onStopPlay();
    void onPaused(bool p);
    void onSpeedChange(qreal speed);
    void setFrameRate();
    void seek();
    void seek(int);
    void showHideVolumeBar();
    void setVolume();
    void tryHideControlBar();
    void tryShowControlBar();
    void showInfo();
    void onPositionChange(qint64 pos);
    void repeatAChanged(const QTime& t);
    void repeatBChanged(const QTime& t);

    void onTimeSliderHover(int pos, int value);
    void onTimeSliderLeave();
    void handleError(const QtAV::AVError& e);
    void onMediaStatusChanged();
    void onBufferProgress(qreal percent);

    void onVideoEQEngineChanged();
    void onBrightnessChanged(int b);
    void onContrastChanged(int c);
    void onHueChanged(int h);
    void onSaturationChanged(int s);

    void onSeekFinished(qint64 pos);
    void onCaptureConfigChanged();
    void onAVFilterVideoConfigChanged();
    void onAVFilterAudioConfigChanged();
    void onBufferValueChanged();
    void onAbortOnTimeoutChanged();

    void onUserShaderChanged();

    void setup();

    void handleFullscreenChange();
    void toggoleSubtitleEnabled(bool value);
    void toggleSubtitleAutoLoad(bool value);
    void openSubtitle();
    void setSubtitleCharset(const QString& charSet);
    void setSubtitleEngine(const QString& value);

    void changeClockType(QAction* action);
    void syncVolumeUi(qreal value);

protected:

    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);

    virtual void closeEvent(QCloseEvent* e);
    virtual void resizeEvent(QResizeEvent* e);
    virtual void timerEvent(QTimerEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void keyReleaseEvent(QKeyEvent* e);

private:

    void workaroundRendererSize();

private:

    bool                            mIsReady;
    bool                            mHasPendingPlay;
    bool                            mControlOn;
    int                             mCursorTimer;
    int                             mShowControl;           ///< 0: can hide, 1: show and playing, 2: always show(not playing)
    int                             mRepeateMax;
    QStringList                     mAudioBackends;
    QVBoxLayout*                    mpPlayerLayout;

    QWidget*                        mpControl;
    QLabel*                         mpCurrent;
    QLabel*                         mpEnd;
    QLabel*                         mpTitle;
    QLabel*                         mpSpeed;
    QtAV::AVPlayerSlider*           mpTimeSlider;
    QtAV::AVPlayerSlider*           mpVolumeSlider;
    QToolButton*                    mpVolumeBtn;
    QToolButton*                    mpPlayPauseBtn;
    QToolButton*                    mpStopBtn;
    QToolButton*                    mpForwardBtn;
    QToolButton*                    mpBackwardBtn;
    QToolButton*                    mpOpenBtn;
    QToolButton*                    mpInfoBtn;
    QToolButton*                    mpMenuBtn;
    QToolButton*                    mpSetupBtn;
    QToolButton*                    mpCaptureBtn;
    QMenu*                          mpMenu;
    QAction*                        mpVOAction; // remove mpVOAction if vo.id() is supported
    QAction*                        mpARAction;
    QAction*                        mpRepeatEnableAction;
    QWidgetAction*                  mpRepeatAction;
    QSpinBox*                       mpRepeatBox;
    QTimeEdit*                      mpRepeatA;
    QTimeEdit*                      mpRepeatB;
    QAction*                        mpAudioTrackAction;
    QMenu*                          mpAudioTrackMenu;
    QMenu*                          mpChannelMenu;
    QAction*                        mpChannelAction;
    QList<QAction*>                 mVOActions;

    QtAV::AVClock*                  mpClock;
    QtAV::AVPlayerCore*             mpPlayer;
    QtAV::VideoRenderer*            mpRenderer;
    QtAV::LibAVFilterVideo*         mpVideoFilter;
    QtAV::LibAVFilterAudio*         mpAudioFilter;
    QString                         mFile;
    QString                         mTitle;

    QLabel*                         mpPreview;

    QtAV::DecoderConfigPage*        mpDecoderConfigPage;
    QtAV::VideoEQConfigPage*        mpVideoEQ;

    PlayList*                       mpPlayList;
    PlayList*                       mpHistory;

    QPointF                         mGlobalMouse;
    StatisticsView*                 mpStatisticsView;

    OSDFilter*                      mpOSD;
    QtAV::SubtitleFilter*           mpSubtitle;
    QtAV::VideoPreviewWidget*       m_preview;
    QtAV::DynamicShaderObject*      m_shader;
    QtAV::GLSLFilter*               m_glsl;
};

} // namespace AVPlayer

#endif // AV_PLAYER_MAIN_WINDOW_H
