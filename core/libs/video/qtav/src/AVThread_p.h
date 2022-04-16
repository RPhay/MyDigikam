/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#ifndef QTAV_AVTHREAD_P_H
#define QTAV_AVTHREAD_P_H

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QSemaphore>
#include <QtCore/QVariant>
#include <QtCore/QWaitCondition>
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
#include <QtCore/QElapsedTimer>
#else
#include <QtCore/QTime>
typedef QTime QElapsedTimer;
#endif

#include "PacketBuffer.h"
#include "utils/ring.h"

QT_BEGIN_NAMESPACE
class QRunnable;
QT_END_NAMESPACE
namespace QtAV
{

const double kSyncThreshold = 0.2; // 200 ms
class AVDecoder;
class AVOutput;
class AVClock;
class Filter;
class Statistics;
class OutputSet;
class AVThreadPrivate : public DPtrPrivate<AVThread>
{
public:
    AVThreadPrivate():
        paused(false)
      , next_pause(false)
      , stop(false)
      , clock(0)
      , dec(0)
      , outputSet(0)
      , delay(0)
      , statistics(0)
      , seek_requested(false)
      , render_pts0(-1)
      , drop_frame_seek(true)
      , pts_history(30)
      , wait_err(0)
    {
        tasks.blockFull(false);

        QVariantHash opt;
        opt[QString::fromLatin1("skip_frame")] = 8; // 8 for "avcodec", "NoRef" for "FFmpeg". see AVDiscard
        dec_opt_framedrop[QString::fromLatin1("avcodec")] = opt;
        opt[QString::fromLatin1("skip_frame")] = 0; // 0 for "avcodec", "Default" for "FFmpeg". see AVDiscard
        dec_opt_normal[QString::fromLatin1("avcodec")] = opt; // avcodec need correct string or value in libavcodec
    }
    virtual ~AVThreadPrivate();

    bool paused, next_pause;
    volatile bool stop; //true when packets is empty and demux is end.
    AVClock *clock;
    PacketBuffer packets;
    AVDecoder *dec;
    OutputSet *outputSet;
    QMutex mutex;
    QWaitCondition cond; //pause
    qreal delay;
    QList<Filter*> filters;
    Statistics *statistics; //not obj. Statistics is unique for the player, which is in AVPlayer
    BlockingQueue<QRunnable*> tasks;
    QSemaphore sem;
    bool seek_requested;
    //only decode video without display or skip decode audio until pts reaches
    qreal render_pts0;

    static QVariantHash dec_opt_framedrop, dec_opt_normal;
    bool drop_frame_seek;
    ring<qreal> pts_history;

    qint64 wait_err;
    QElapsedTimer wait_timer;
};

} // namespace QtAV
#endif // QTAV_AVTHREAD_P_H
