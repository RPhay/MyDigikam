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
    Q_OBJECT
    Q_DISABLE_COPY(VideoDecoderFFmpegBase)
    DPTR_DECLARE_PRIVATE(VideoDecoderFFmpegBase)

public:

    virtual bool decode(const Packet& packet)   override;
    virtual VideoFrame frame()                  override;

protected:

    explicit VideoDecoderFFmpegBase(VideoDecoderFFmpegBasePrivate& d);

private:

    VideoDecoderFFmpegBase(); // it's a base class
    VideoDecoderFFmpegBase(QObject*);
};

// -----------------------------------------------------------------

class VideoDecoderFFmpegBasePrivate : public VideoDecoderPrivate
{
public:

    VideoDecoderFFmpegBasePrivate()
        : VideoDecoderPrivate(),
          frame              (nullptr),
          width              (0),
          height             (0)
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
    qreal getDAR(AVFrame* f);

public:

    AVFrame* frame  = nullptr;          ///< set once and not change
    int      width  = 0;                ///< The current decoded frame size
    int      height = 0;                ///< The current decoded frame size

private:

    Q_DISABLE_COPY(VideoDecoderFFmpegBasePrivate);
};

} // namespace QtAV

#endif // QTAV_VIDEO_DECODER_FFMPEG_BASE_H
