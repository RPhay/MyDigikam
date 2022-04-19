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

#ifndef QTAV_VIDEO_DECODER_FFMPEG_BASE_H
#define QTAV_VIDEO_DECODER_FFMPEG_BASE_H

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
    virtual bool decode(const Packet& packet) Q_DECL_OVERRIDE;
    virtual VideoFrame frame() Q_DECL_OVERRIDE;
protected:
    VideoDecoderFFmpegBase(VideoDecoderFFmpegBasePrivate &d);
private:
    VideoDecoderFFmpegBase(); //it's a base class
};

class VideoDecoderFFmpegBasePrivate : public VideoDecoderPrivate
{
public:
    VideoDecoderFFmpegBasePrivate()
        : VideoDecoderPrivate()
        , frame(0)
        , width(0)
        , height(0)
    {
#if !AVCODEC_STATIC_REGISTER
        avcodec_register_all();
#endif
        frame = av_frame_alloc();
    }
    virtual ~VideoDecoderFFmpegBasePrivate() {
        if (frame) {
            av_frame_free(&frame);
            frame = 0;
        }
    }
    void updateColorDetails(VideoFrame* f);
    qreal getDAR(AVFrame *f);

    AVFrame *frame; //set once and not change
    int width, height; //The current decoded frame size
};

} // namespace QtAV

#endif // QTAV_VIDEO_DECODER_FFMPEG_BASE_H
