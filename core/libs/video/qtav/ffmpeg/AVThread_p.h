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

#ifndef QTAV_AV_THREAD_P_H
#define QTAV_AV_THREAD_P_H

#include "AVThread.h"

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QSemaphore>
#include <QVariant>
#include <QWaitCondition>
#include <QElapsedTimer>

// Local includes

#include "PacketBuffer.h"
#include "QtAV_ring.h"

class QRunnable;

namespace QtAV
{

const double kSyncThreshold = 0.2; // 200 ms

class AVDecoder;
class AVOutput;
class AVClock;
class Filter;
class Statistics;
class OutputSet;

class Q_DECL_HIDDEN AVThreadPrivate : public DPtrPrivate<AVThread>
{
public:

    AVThreadPrivate()
      : paused          (false),
        next_pause      (false),
        stop            (false),
        clock           (nullptr),
        dec             (nullptr),
        outputSet       (nullptr),
        delay           (0),
        statistics      (nullptr),
        seek_requested  (false),
        render_pts0     (-1),
        drop_frame_seek (true),
        pts_history     (30),
        wait_err        (0)
    {
        tasks.blockFull(false);

        QVariantHash opt;
        opt[QString::fromLatin1("skip_frame")]            = 8;   // 8 for "avcodec", "NoRef" for "FFmpeg". see AVDiscard
        dec_opt_framedrop[QString::fromLatin1("avcodec")] = opt;
        opt[QString::fromLatin1("skip_frame")]            = 0;   // 0 for "avcodec", "Default" for "FFmpeg". see AVDiscard
        dec_opt_normal[QString::fromLatin1("avcodec")]    = opt; // avcodec need correct string or value in libavcodec
    }

    virtual ~AVThreadPrivate();

public:

    bool                        paused;
    bool                        next_pause;
    volatile bool               stop;       // true when packets is empty and demux is end.
    AVClock*                    clock;
    PacketBuffer                packets;
    AVDecoder*                  dec;
    OutputSet*                  outputSet;
    QMutex                      mutex;
    QWaitCondition              cond;       // pause
    qreal                       delay;
    QList<Filter*>              filters;
    Statistics*                 statistics; // not obj. Statistics is unique for the player, which is in AVPlayerCore
    BlockingQueue<QRunnable*>   tasks;
    QSemaphore                  sem;
    bool                        seek_requested;

    // only decode video without display or skip decode audio until pts reaches

    qreal                       render_pts0;

    static QVariantHash         dec_opt_framedrop;
    static QVariantHash         dec_opt_normal;
    bool                        drop_frame_seek;
    ring<qreal>                 pts_history;

    qint64                      wait_err;
    QElapsedTimer               wait_timer;
};

} // namespace QtAV

#endif // QTAV_AV_THREAD_P_H
