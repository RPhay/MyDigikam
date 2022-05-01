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

#ifndef QTAV_DEMUX_THREAD_H
#define QTAV_DEMUX_THREAD_H

// Qt includes

#include <QMutex>
#include <QSemaphore>
#include <QThread>
#include <QRunnable>
#include <QTimer>

// Local includes

#include "PacketBuffer.h"

namespace QtAV
{

class AVDemuxer;
class AVThread;

class AVDemuxThread : public QThread
{
    Q_OBJECT

public:

    explicit AVDemuxThread(QObject *parent = nullptr);
    explicit AVDemuxThread(AVDemuxer *dmx, QObject *parent = nullptr);

    void setDemuxer(AVDemuxer *dmx);
    void setAudioDemuxer(AVDemuxer *demuxer);   ///< not thread safe
    void setAudioThread(AVThread *thread);
    AVThread* audioThread();
    void setVideoThread(AVThread *thread);
    AVThread* videoThread();
    void stepForward();                         ///< show next video frame and pause
    void stepBackward();
    void seek(qint64 external_pos, qint64 pos, SeekType type); //ms

    //AVDemuxer* demuxer

    bool isPaused() const;
    bool isEnd() const;
    bool atEndOfMedia() const;
    PacketBuffer* buffer();
    void updateBufferState();
    void stop();                                ///< TODO: remove it?
    void pause(bool p, bool wait = false);

    MediaEndAction mediaEndAction() const;
    void setMediaEndAction(MediaEndAction value);
    bool waitForStarted(int msec = -1);
    qint64 lastSeekPos();
    bool hasSeekTasks();

Q_SIGNALS:

    void requestClockPause(bool value);
    void mediaEndActionPauseTriggered();
    void mediaStatusChanged(QtAV::MediaStatus);
    void bufferProgressChanged(qreal);
    void seekFinished(qint64 timestamp);
    void stepFinished();
    void internalSubtitlePacketRead(int index, const QtAV::Packet& packet);

private Q_SLOTS:

    void finishedStepBackward();
    void seekOnPauseFinished();
    void frameDeliveredOnStepForward();
    void eofDecodedOnStepForward();
    void stepForwardDone();
    void onAVThreadQuit();

protected:

    virtual void run();

    /**
     * If the pause state is true set by pause(true), then block the thread and wait for pause state changed, i.e. pause(false)
     * and return true. Otherwise, return false immediatly.
     */
    bool tryPause(unsigned long timeout = 100);

private:

    void setAVThread(AVThread *&pOld, AVThread* pNew);
    void newSeekRequest(QRunnable *r);
    void processNextSeekTask();
    void seekInternal(qint64 pos, SeekType type, qint64 external_pos = std::numeric_limits < qint64 >::min()); //must call in AVDemuxThread
    void pauseInternal(bool value);

private:

    bool                        paused;
    bool                        user_paused;
    volatile bool               end;
    MediaEndAction              end_action;
    bool                        m_buffering;
    PacketBuffer*               m_buffer;
    AVDemuxer*                  demuxer;
    AVDemuxer*                  ademuxer;
    AVThread*                   audio_thread, *video_thread;
    int                         clock_type;         ///< change happens in different threads(direct connection)
    qint64                      last_seek_pos;
    QRunnable*                  current_seek_task;
    bool                        stepping;
    qint64                      stepping_timeout_time;

    int                         audio_stream, video_stream;
    QMutex                      buffer_mutex;
    QWaitCondition              cond;
    BlockingQueue<QRunnable*>   seek_tasks;

    QSemaphore                  sem;
    QMutex                      next_frame_mutex;

private:

    friend class SeekTask;
    friend class stepBackwardTask;
};

} // namespace QtAV

#endif // QTAV_DEMUX_THREAD_H
