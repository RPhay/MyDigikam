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

#include "OutputSet.h"

// Local includes

#include "AVPlayerCore.h"
#include "VideoRenderer.h"

namespace QtAV
{

OutputSet::OutputSet(AVPlayerCore* const player)
    : QObject        (player),
      mCanPauseThread(false),
      mpPlayer       (player),
      mPauseCount    (0)
{
}

OutputSet::~OutputSet()
{
    mCond.wakeAll();

    // delete? may be deleted by vo's parent

    clearOutputs();
}

void OutputSet::lock()
{
    mMutex.lock();
}

void OutputSet::unlock()
{
    mMutex.unlock();
}

QList<AVOutput*> OutputSet::outputs()
{
    return mOutputs;
}

void OutputSet::sendVideoFrame(const VideoFrame& frame)
{
    if (mOutputs.isEmpty())
        return;

    VideoFrame f(frame);

    Q_FOREACH (AVOutput* const output, mOutputs)
    {
        if (!output->isAvailable())
            continue;

        VideoRenderer* const vo = static_cast<VideoRenderer*>(output);

        // TODO: sort vo by supported formats when a new vo is added to reduce convertion

        if (!vo->isSupported(frame.pixelFormat()))
            f = frame.to(vo->preferredPixelFormat());

        vo->receive(f);
    }
}

void OutputSet::clearOutputs()
{
    QMutexLocker lock(&mMutex);
    Q_UNUSED(lock);

    if (mOutputs.isEmpty())
        return;

    Q_FOREACH (AVOutput* const output, mOutputs)
    {
        output->removeOutputSet(this);
    }

    mOutputs.clear();
}

void OutputSet::addOutput(AVOutput* output)
{
    QMutexLocker lock(&mMutex);
    Q_UNUSED(lock);

    mOutputs.append(output);
    output->addOutputSet(this);
}

void OutputSet::removeOutput(AVOutput* output)
{
    QMutexLocker lock(&mMutex);
    Q_UNUSED(lock);

    mOutputs.removeAll(output);
    output->removeOutputSet(this);
}

void OutputSet::notifyPauseChange(AVOutput* output)
{
    if (output->isPaused())
    {
        mPauseCount++;

        if (mPauseCount == mOutputs.size())
        {
            mCanPauseThread = true;
        }

        // DO NOT pause here because it must be paused in AVThread
    }
    else
    {
        mPauseCount--;
        mCanPauseThread = false;

        if (mPauseCount == (mOutputs.size() - 1))
        {
            resumeThread();
        }
    }
}

bool OutputSet::canPauseThread() const
{
    return mCanPauseThread;
}

bool OutputSet::pauseThread(unsigned long timeout)
{
    QMutexLocker lock(&mMutex);
    Q_UNUSED(lock);

    return mCond.wait(&mMutex, timeout);
}

void OutputSet::resumeThread()
{
    mCond.wakeAll();
}

} // namespace QtAV
