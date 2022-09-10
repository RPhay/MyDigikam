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

class AVPlayerCore;
class VideoFrame;

class OutputSet : public QObject
{
    Q_OBJECT

public:

    explicit OutputSet(AVPlayerCore* const player);
    virtual ~OutputSet();

    // required when accessing renderers

    void lock();
    void unlock();

    // implicity shared
/*
    QList<AVOutput*> outputs();
*/
    QList<AVOutput*> outputs();

    // each(OutputOperation(data))

    void sendData(const QByteArray& data);
    void sendVideoFrame(const VideoFrame& frame);

    void clearOutputs();
    void addOutput(AVOutput* output);

    void notifyPauseChange(AVOutput* output);
    bool canPauseThread() const;

    /**
     * in AVThread
     * There are 2 ways to pause AVThread: 1. pause thread directly. 2. pause all outputs
     */
    bool pauseThread(unsigned long timeout = ULONG_MAX);

    /*
     * in user thread when pause count < set size.
     * 1. AVPlayerCore.pause(false) in player thread then call each output pause(false)
     * 2. shortcut for AVOutput.pause(false)
     */
    void resumeThread();

public Q_SLOTS:

    // connect to renderer->aboutToClose(). test whether delete on close

    void removeOutput(AVOutput* output);

private:

    OutputSet(QObject*);

private:

    volatile bool    mCanPauseThread;
    AVPlayerCore*    mpPlayer;
    int              mPauseCount;   ///< pause AVThread if equals to mOutputs.size()
    QList<AVOutput*> mOutputs;
    QMutex           mMutex;
    QWaitCondition   mCond;         // pause
};

} // namespace QtAV

#endif // QTAV_OUTPUT_SET_H
