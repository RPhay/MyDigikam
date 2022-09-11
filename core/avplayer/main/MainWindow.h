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

#ifndef AV_PLAYER_MAIN_WINDOW_H
#define AV_PLAYER_MAIN_WINDOW_H

// Qt includes

#include <QWidget>
#include <QUrl>

namespace QtAV
{
    class VideoRenderer;
    class AVError;
}

namespace AVPlayer
{

class MainWindow : public QWidget
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget* const parent = nullptr);
    ~MainWindow();

Q_SIGNALS:

    void ready();

//@{
/// Setup methods: MainWindow_setup.cpp

public:

    void setAudioBackends(const QStringList& backends);
    bool setRenderer(QtAV::VideoRenderer* const renderer);
    void setVideoDecoderNames(const QStringList& vd);

private Q_SLOTS:

    void initPlayer();
    void initAudioTrackMenu();
    void setupUi();
    void setFrameRate();
    void setVolume();
    void setup();
    void changeClockType(QAction* action);
    void setRepeateMax(int);
    void changeVO(QAction* action);
    void changeChannel(QAction* action);
    void changeAudioTrack(QAction* action);

//@{
/// File IO methods: MainWindow_io.cpp

public Q_SLOTS:

    void play(const QString& name);
    void play(const QUrl& url);
    void openFile();

private Q_SLOTS:

    void stopUnload();
    void openUrl();
    void playOnlineVideo(QAction* action);
    void seek();
    void seek(int);
    void onSeekFinished(qint64 pos);
    void openSubtitle();

//@}

//@{
/// Control methods: MainWindow_control.cpp

public Q_SLOTS:

    void togglePlayPause();

private Q_SLOTS:

    void toggleRepeat(bool);
    void onPlayListClick(const QString& key, const QString& value);
    void onStartPlay();
    void onStopPlay();
    void onPaused(bool p);
    void onSpeedChange(qreal speed);
    void repeatAChanged(const QTime& t);
    void repeatBChanged(const QTime& t);
    void handleFullscreenChange();
    void toggoleSubtitleEnabled(bool value);
    void toggleSubtitleAutoLoad(bool value);

//@}

//@{
/// Event methods: MainWindow_events.cpp

protected:

    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);

protected:

    virtual void closeEvent(QCloseEvent* e);
    virtual void resizeEvent(QResizeEvent* e);
    virtual void timerEvent(QTimerEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void keyReleaseEvent(QKeyEvent* e);

//@}

//@{
/// Render methods: MainWindow_render.cpp

public Q_SLOTS:

    void showNextOSD();

private Q_SLOTS:

    void switchAspectRatio(QAction* action);

//@}

//@{
/// Configuration methods: MainWindow_config.cpp

//@}

private Q_SLOTS:

    void about();
    void updateChannelMenu();
    void processPendingActions();
    void onSpinBoxChanged(double v);
    void showHideVolumeBar();
    void tryHideControlBar();
    void tryShowControlBar();
    void showInfo();
    void onPositionChange(qint64 pos);

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

    void onCaptureConfigChanged();
    void onAVFilterVideoConfigChanged();
    void onAVFilterAudioConfigChanged();
    void onBufferValueChanged();
    void onAbortOnTimeoutChanged();

    void onUserShaderChanged();

    void setSubtitleCharset(int);
    void setSubtitleEngine(int);

    void syncVolumeUi(qreal value);


private:

    class Private;
    Private* const d;
};

} // namespace AVPlayer

#endif // AV_PLAYER_MAIN_WINDOW_H
