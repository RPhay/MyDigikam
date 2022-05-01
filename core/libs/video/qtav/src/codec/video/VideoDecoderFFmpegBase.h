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

#ifndef QTAV_VIDEO_DECODER_FFMPEG_BASE_H
#define QTAV_VIDEO_DECODER_FFMPEG_BASE_H

// Local includes

#include "VideoDecoder.h"
#include "AVDecoder_p.h"
#include "AVCompat.h"

namespace QtAV
{

class VideoDecoderFFmpegBasePrivate;

class VideoDecoderFFmpegBase : public VideoDecoder
{
    Q_DISABLE_COPY(VideoDecoderFFmpegBase)
    DPTR_DECLARE_PRIVATE(VideoDecoderFFmpegBase)

public:

    virtual bool decode(const Packet& packet) override;
    virtual VideoFrame frame() override;

protected:

    explicit VideoDecoderFFmpegBase(VideoDecoderFFmpegBasePrivate &d);

private:

    VideoDecoderFFmpegBase(); // it's a base class
};

class VideoDecoderFFmpegBasePrivate : public VideoDecoderPrivate
{
public:

    VideoDecoderFFmpegBasePrivate()
        : VideoDecoderPrivate()
        , frame(nullptr)
        , width(0)
        , height(0)
    {

#if !AVCODEC_STATIC_REGISTER

        avcodec_register_all();

#endif

        frame = av_frame_alloc();
    }

    virtual ~VideoDecoderFFmpegBasePrivate()
    {
        if (frame)
        {
            av_frame_free(&frame);
            frame = nullptr;
        }
    }

    void updateColorDetails(VideoFrame* f);
    qreal getDAR(AVFrame *f);

    AVFrame* frame;          // set once and not change
    int      width, height;  // The current decoded frame size
};

} // namespace QtAV

#endif // QTAV_VIDEO_DECODER_FFMPEG_BASE_H
