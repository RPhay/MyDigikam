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

#ifndef QTAV_VIDEO_DECODER_FFMPEG_HW_P_H
#define QTAV_VIDEO_DECODER_FFMPEG_HW_P_H

// Local includes

#include "VideoDecoderFFmpegHW.h"
#include "GPUMemCopy.h"

/*!
 * QTAV_HAVE(AVBUFREF): use AVCodecContext.get_buffer2 instead of old callbacks.
 * In order to avoid compile warnings, now disable old callbacks if possible.
 * maybe we can also do a runtime check and enable old callbacks
 */

namespace QtAV
{

class Q_DECL_HIDDEN VideoDecoderFFmpegHWPrivate : public VideoDecoderFFmpegBasePrivate
{
public:

    VideoDecoderFFmpegHWPrivate()
        : VideoDecoderFFmpegBasePrivate(),
          get_format                   (nullptr),
          get_buffer                   (nullptr),
          release_buffer               (nullptr),
          reget_buffer                 (nullptr),
          get_buffer2                  (nullptr),
          threads                      (0),
          copy_mode                    (VideoDecoderFFmpegHW::OptimizedCopy),
          hw_w                         (0),
          hw_h                         (0),
          hw_profile                   (0)
    {
    }

    virtual ~VideoDecoderFFmpegHWPrivate()
    {
        // ctx is 0 now
    }

    bool enableFrameRef() const override
    {
        return false; // because of ffmpeg_get_va_buffer2?
    }

    bool prepare();

    void restore()
    {
        codec_ctx->pix_fmt        = pixfmt;
        codec_ctx->opaque         = nullptr;
        codec_ctx->get_format     = get_format;

#if QTAV_HAVE(AVBUFREF)

        codec_ctx->get_buffer2    = get_buffer2;

#else

        codec_ctx->get_buffer     = get_buffer;
        codec_ctx->release_buffer = release_buffer;
        codec_ctx->reget_buffer   = reget_buffer;
#endif

    }

    virtual bool open() override
    {
        return prepare();
    }

    virtual void close() override
    {
        restore();
    }

    // return hwaccel_context or null

    virtual void* setup(AVCodecContext* avctx) = 0;

    AVPixelFormat getFormat(struct AVCodecContext* p_context, const AVPixelFormat* pi_fmt);

    // TODO: remove opaque

    virtual bool getBuffer(void** opaque, uint8_t** data)         = 0;
    virtual void releaseBuffer(void* opaque, uint8_t* data)       = 0;
    virtual AVPixelFormat vaPixelFormat()                   const = 0;

    int codedWidth(AVCodecContext* avctx)                   const;  // TODO: virtual int surfaceWidth(AVCodecContext*) const;
    int codedHeight(AVCodecContext* avctx)                  const;
    bool initUSWC(int lineSize);
    void releaseUSWC();

    AVPixelFormat pixfmt; // store old one

    // store old values because it does not own AVCodecContext

    AVPixelFormat (*get_format)(struct AVCodecContext* s, const AVPixelFormat* fmt);
    int (*get_buffer)(struct AVCodecContext* c, AVFrame* pic);
    void (*release_buffer)(struct AVCodecContext* c, AVFrame* pic);
    int (*reget_buffer)(struct AVCodecContext* c, AVFrame* pic);
    int (*get_buffer2)(struct AVCodecContext* s, AVFrame* frame, int flags);

public:

    QString                         description;
    int                             threads; ///< multithread decoding may crash for some decoders (dxva, videotoolbox)

    // false for not intel gpu. my test result is intel gpu is supper
    // fast and lower cpu usage if use optimized uswc copy. but nv is worse.
    // TODO: flag enable, disable, auto

    VideoDecoderFFmpegHW::CopyMode  copy_mode;
    GPUMemCopy                      gpu_mem;

private:

    int                             hw_w;
    int                             hw_h;
    int                             hw_profile;
};

} // namespace QtAV

#endif // QTAV_VIDEO_DECODER_FFMPEG_HW_P_H
