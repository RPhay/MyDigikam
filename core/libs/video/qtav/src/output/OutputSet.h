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

#ifndef QTAV_OUTPUT_SET_H
#define QTAV_OUTPUT_SET_H

// C++ includes

#include <climits>

// Qt includes

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

// Local includes

#include "QtAV_Global.h"
#include "AVOutput.h"

namespace QtAV
{

class AVPlayer;
class VideoFrame;

class OutputSet : public QObject
{
    Q_OBJECT

public:

    OutputSet(AVPlayer *player);
    virtual ~OutputSet();

    // required when accessing renderers

    void lock();
    void unlock();

    // implicity shared

    //QList<AVOutput*> outputs();

    QList<AVOutput*> outputs();

    // each(OutputOperation(data))

    void sendData(const QByteArray& data);
    void sendVideoFrame(const VideoFrame& frame);

    void clearOutputs();
    void addOutput(AVOutput* output);

    void notifyPauseChange(AVOutput *output);
    bool canPauseThread() const;

    // in AVThread

    bool pauseThread(unsigned long timeout = ULONG_MAX); // There are 2 ways to pause AVThread: 1. pause thread directly. 2. pause all outputs

    /*
     * in user thread when pause count < set size.
     * 1. AVPlayer.pause(false) in player thread then call each output pause(false)
     * 2. shortcut for AVOutput.pause(false)
     */
    void resumeThread();

public Q_SLOTS:

    // connect to renderer->aboutToClose(). test whether delete on close

    void removeOutput(AVOutput *output);

private:

    volatile bool    mCanPauseThread;
    AVPlayer*        mpPlayer;
    int              mPauseCount;   // pause AVThread if equals to mOutputs.size()
    QList<AVOutput*> mOutputs;
    QMutex           mMutex;
    QWaitCondition   mCond;         // pause
};

} // namespace QtAV

#endif // QTAV_OUTPUT_SET_H
