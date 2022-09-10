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

#ifndef QTAV_VIDEO_THREAD_H
#define QTAV_VIDEO_THREAD_H

#include "AVThread.h"

// Qt includes

#include <QSize>

namespace QtAV
{

class VideoCapture;
class VideoFrame;
class VideoThreadPrivate;

class VideoThread : public AVThread
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(VideoThread)

public:

    explicit VideoThread(QObject* const parent = nullptr);
    VideoCapture* setVideoCapture(VideoCapture* const cap);       // ensure thread safe
    VideoCapture* videoCapture() const;
    VideoFrame displayedFrame()  const;
    void setFrameRate(qreal value);

    //virtual bool event(QEvent *event);

    void setBrightness(int val);
    void setContrast(int val);
    void setSaturation(int val);
    void setEQ(int b, int c, int s);

public Q_SLOTS:

    void addCaptureTask();
    void clearRenderers();

protected:

    void applyFilters(VideoFrame& frame);

    // deliver video frame to video renderers. frame may be converted to a suitable format for renderer

    bool deliverVideoFrame(VideoFrame& frame);
    virtual void run();

    // wait for value msec. every usleep is a small time, then process next task and get new delay
};

} // namespace QtAV

#endif // QTAV_VIDEO_THREAD_H
