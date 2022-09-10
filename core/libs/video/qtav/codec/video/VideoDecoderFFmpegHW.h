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

#ifndef QTAV_VIDEO_DECODER_FFMPEG_HW_H
#define QTAV_VIDEO_DECODER_FFMPEG_HW_H

#include "VideoDecoderFFmpegBase.h"

namespace QtAV
{

class VideoDecoderFFmpegHWPrivate;

class VideoDecoderFFmpegHW : public VideoDecoderFFmpegBase
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoDecoderFFmpegHW)
    DPTR_DECLARE_PRIVATE(VideoDecoderFFmpegHW)
    Q_PROPERTY(int threads READ threads WRITE setThreads NOTIFY threadsChanged) // <= 0 is auto
    Q_PROPERTY(CopyMode copyMode READ copyMode WRITE setCopyMode NOTIFY copyModeChanged)

public:

    enum CopyMode
    {
        ZeroCopy,
        OptimizedCopy,
        GenericCopy
    };
    Q_ENUM(CopyMode)

public:

    VideoFrame copyToFrame(const VideoFormat& fmt,
                           int surface_h,
                           quint8* src[],
                           int pitch[],
                           bool swapUV);

    // properties

    int threads()                       const;
    void setThreads(int value);
    void setCopyMode(CopyMode value);
    CopyMode copyMode()                 const;

Q_SIGNALS:

    void copyModeChanged();
    void threadsChanged();

protected:

    explicit VideoDecoderFFmpegHW(VideoDecoderFFmpegHWPrivate& d);

private:

    // Disable

    VideoDecoderFFmpegHW();
    VideoDecoderFFmpegHW(QObject*);
};

} // namespace QtAV

#endif // QTAV_VIDEO_DECODER_FFMPEG_HW_H
