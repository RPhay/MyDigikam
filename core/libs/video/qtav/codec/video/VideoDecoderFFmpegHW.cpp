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

#include "VideoDecoderFFmpegHW_p.h"

// C++ includes

#include <algorithm>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace QtAV
{

static AVPixelFormat ffmpeg_get_va_format(struct AVCodecContext* c, const AVPixelFormat* ff)
{
    VideoDecoderFFmpegHWPrivate* const va = reinterpret_cast<VideoDecoderFFmpegHWPrivate*>(c->opaque);

    return va->getFormat(c, ff);
}

#if QTAV_HAVE(AVBUFREF)

typedef struct Q_DECL_HIDDEN ffmpeg_va_ref_t
{
    VideoDecoderFFmpegHWPrivate* va     = nullptr;
    void*                        opaque = nullptr;           // va surface from AVFrame.opaque
} ffmpeg_va_ref_t;

static void ffmpeg_release_va_buffer2(void* opaque, uint8_t* data)
{
    ffmpeg_va_ref_t* const ref = (ffmpeg_va_ref_t*)opaque;
    ref->va->releaseBuffer(ref->opaque, data);

    delete ref;
}

static int ffmpeg_get_va_buffer2(struct AVCodecContext* ctx, AVFrame* frame, int flags)
{
    Q_UNUSED(flags);

    for (unsigned i = 0 ; i < AV_NUM_DATA_POINTERS ; ++i)
    {
        frame->data[i]     = nullptr;
        frame->linesize[i] = 0;
        frame->buf[i]      = nullptr;
    }
/*
    frame->reordered_opaque = ctx->reordered_opaque; //?? xbmc
*/
    // va must be available here

    VideoDecoderFFmpegHWPrivate* const va = (VideoDecoderFFmpegHWPrivate*)ctx->opaque;

    if (!va->getBuffer(&frame->opaque, &frame->data[0]))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("va->getBuffer failed");

        return -1;
    }

    ffmpeg_va_ref_t* const ref = new ffmpeg_va_ref_t;
    ref->va                    = va;
    ref->opaque                = frame->opaque;

    // data[0] must be non-nullptr for libavcodec internal checks. data[3] actually contains
    // the format-specific surface handle.

    frame->data[3]             = frame->data[0];
    frame->buf[0]              = av_buffer_create(frame->data[0], 0, ffmpeg_release_va_buffer2, ref, 0);

    if (Q_UNLIKELY(!frame->buf[0]))
    {
        ffmpeg_release_va_buffer2(ref, frame->data[0]);

        return -1;
    }

    Q_ASSERT(frame->data[0] != nullptr);

    return 0;
}

#else

static int ffmpeg_get_va_buffer(struct AVCodecContext* c, AVFrame* ff) // vlc_va_t *external, AVFrame *ff)
{
    VideoDecoderFFmpegHWPrivate* const va = reinterpret_cast<VideoDecoderFFmpegHWPrivate*>(c->opaque);
/*
    ff->reordered_opaque = c->reordered_opaque; // TODO: dxva?
*/
    ff->opaque = 0;

#   if !AV_MODULE_CHECK(LIBAVCODEC, 54, 34, 0, 79, 101)

    ff->pkt_pts = (c->pkt ? c->pkt->pts : AV_NOPTS_VALUE);

#   endif

#   if LIBAVCODEC_VERSION_MAJOR < 54

    ff->age = 256 * 256 * 256 * 64;

#   endif

    // hwaccel_context is not present in old ffmpeg version
    // va must be available here

    if (!va->getBuffer(&ff->opaque, &ff->data[0]))
        return -1;

    // ffmpeg_va_GetFrameBuf

    ff->data[3] = ff->data[0];
    ff->type    = FF_BUFFER_TYPE_USER;

    return 0;
}

static void ffmpeg_release_va_buffer(struct AVCodecContext* c, AVFrame* ff)
{
    VideoDecoderFFmpegHWPrivate* const va = reinterpret_cast<VideoDecoderFFmpegHWPrivate*>(c->opaque);
    va->releaseBuffer(ff->opaque, ff->data[0]);
    memset(ff->data,     0, sizeof(ff->data));
    memset(ff->linesize, 0, sizeof(ff->linesize));
}

#endif // QTAV_HAVE(AVBUFREF)

bool VideoDecoderFFmpegHWPrivate::prepare()
{
    // From vlc begin

#   if LIBAVCODEC_VERSION_MAJOR < 60
    codec_ctx->thread_safe_callbacks = true;
#   endif
    codec_ctx->thread_count          = threads;

#ifdef _MSC_VER                  // krazy:exclude=cpp
#   pragma warning(disable:4065) // vc: switch has default but no case
#endif

    switch (codec_ctx->codec_id)
    {

# if (LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 1, 0))

        // tested libav-9.x + va-api. If remove this code: bug detected, please report the issue.
        // Context scratch buffers could not be allocated due to unknown size

        case QTAV_CODEC_ID(H264):
        case QTAV_CODEC_ID(VC1):
        case QTAV_CODEC_ID(WMV3):
        {
            codec_ctx->thread_type &= ~FF_THREAD_FRAME;

            break;
        }
# endif

        default:
        {
            break;
        }
    }

    // From vlc end

    codec_ctx->opaque         = this;

    pixfmt                    = codec_ctx->pix_fmt;
    get_format                = codec_ctx->get_format;

#if QTAV_HAVE(AVBUFREF)

    get_buffer2               = codec_ctx->get_buffer2;

#else

    get_buffer                = codec_ctx->get_buffer;
    reget_buffer              = codec_ctx->reget_buffer;
    release_buffer            = codec_ctx->release_buffer;

#endif // QTAV_HAVE(AVBUFREF)

    codec_ctx->get_format     = ffmpeg_get_va_format;

#if QTAV_HAVE(AVBUFREF)

    codec_ctx->get_buffer2    = ffmpeg_get_va_buffer2;

#else

    // TODO: FF_API_GET_BUFFER

    codec_ctx->get_buffer     = ffmpeg_get_va_buffer;           // ffmpeg_GetFrameBuf;
    codec_ctx->reget_buffer   = avcodec_default_reget_buffer;
    codec_ctx->release_buffer = ffmpeg_release_va_buffer;       // ffmpeg_ReleaseFrameBuf;

#endif // QTAV_HAVE(AVBUFREF)

    return true;
}

AVPixelFormat VideoDecoderFFmpegHWPrivate::getFormat(struct AVCodecContext* avctx, const AVPixelFormat* pi_fmt)
{

#ifdef AV_HWACCEL_FLAG_ALLOW_SOFTWARE

    avctx->hwaccel_flags |= AV_HWACCEL_FLAG_ALLOW_SOFTWARE;

#endif

    bool can_hwaccel      = false;

    for (size_t i = 0 ; pi_fmt[i] != QTAV_PIX_FMT_C(NONE) ; ++i)
    {
        const AVPixFmtDescriptor* dsc = av_pix_fmt_desc_get(pi_fmt[i]);

        if (dsc == nullptr)
            continue;

        bool hwaccel = ((dsc->flags & AV_PIX_FMT_FLAG_HWACCEL) != 0);

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("available %sware decoder output format %d (%s)",
                hwaccel ? "hard" : "soft", pi_fmt[i], dsc->name);

        if (hwaccel)
            can_hwaccel = true;
    }

    if (!can_hwaccel)
        goto end;

    for (size_t i = 0 ; pi_fmt[i] != QTAV_PIX_FMT_C(NONE) ; ++i)
    {
        if (vaPixelFormat() != pi_fmt[i])
            continue;

        if ((hw_w == codedWidth((avctx)))  &&
            (hw_h == codedHeight(avctx))   &&
            (hw_profile == avctx->profile) && // update decoder if profile changed. but now only surfaces are updated
            avctx->hwaccel_context)
        {
            return pi_fmt[i];
        }

        // TODO: manage uswc here for x86 (surface size is decoder dependent)

        avctx->hwaccel_context = setup(avctx);

        if (!avctx->hwaccel_context)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("acceleration setup failure");

            break;
        }

        hw_w       = codedWidth((avctx));
        hw_h       = codedHeight(avctx);
        hw_profile = avctx->profile;

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("Using %s for hardware decoding.",
                qPrintable(description));

        return pi_fmt[i];
    }

    close();

end:

    qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
        << QString::asprintf("hardware acceleration is not available" );

    /* Fallback to default behaviour */

#if QTAV_HAVE(AVBUFREF)

    avctx->get_buffer2 = avcodec_default_get_buffer2;

#endif

    return avcodec_default_get_format(avctx, pi_fmt);
}

int VideoDecoderFFmpegHWPrivate::codedWidth(AVCodecContext* avctx) const
{
    if (avctx->coded_width > 0)
        return avctx->coded_width;

    return avctx->width;
}

int VideoDecoderFFmpegHWPrivate::codedHeight(AVCodecContext* avctx) const
{
    if (avctx->coded_height > 0)
        return avctx->coded_height;

    return avctx->height;
}

bool VideoDecoderFFmpegHWPrivate::initUSWC(int lineSize)
{
    if (copy_mode != VideoDecoderFFmpegHW::OptimizedCopy)
        return false;

    return gpu_mem.initCache(lineSize);
}

void VideoDecoderFFmpegHWPrivate::releaseUSWC()
{
    if (copy_mode == VideoDecoderFFmpegHW::OptimizedCopy)
        gpu_mem.cleanCache();
}

VideoDecoderFFmpegHW::VideoDecoderFFmpegHW(VideoDecoderFFmpegHWPrivate& d)
    : VideoDecoderFFmpegBase(d)
{
    setProperty("detail_copyMode", QString::fromUtf8("%1. %2\n%3. %4\n%5\n%6")
                .arg(i18n("ZeroCopy: fastest. Direct rendering without data copy between CPU and GPU"))
                .arg(i18n("Not implemented for all codecs"))
                .arg(i18n("Not implemented for all codecs"))
                .arg(i18n("OptimizedCopy: copy from USWC memory optimized by SSE4.1"))
                .arg(i18n("GenericCopy: slowest. Generic cpu copy")));

    setProperty("detail_threads", QString::fromUtf8("%1\n%2\n%3\n%4")
                .arg(i18n("Number of decoding threads. Set before open. Maybe no effect for some decoders"))
                .arg(i18n("Multithread decoding may crash"))
                .arg(i18n("0: auto"))
                .arg(i18n("1: single thread decoding")));
}

void VideoDecoderFFmpegHW::setThreads(int value)
{
    DPTR_D(VideoDecoderFFmpegHW);

    if (d.threads == value)
        return;

    d.threads = value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "threads", (int64_t)value, 0);

    Q_EMIT threadsChanged();
}

int VideoDecoderFFmpegHW::threads() const
{
    return d_func().threads;
}

void VideoDecoderFFmpegHW::setCopyMode(CopyMode value)
{
    DPTR_D(VideoDecoderFFmpegHW); // cppcheck-suppress constVariable

    if (d.copy_mode == value)
        return;

    d_func().copy_mode = value;

    Q_EMIT copyModeChanged();
}

VideoDecoderFFmpegHW::CopyMode VideoDecoderFFmpegHW::copyMode() const
{
    return d_func().copy_mode;
}

VideoFrame VideoDecoderFFmpegHW::copyToFrame(const VideoFormat& fmt, int surface_h,
                                             quint8* src[], int pitch[], bool swapUV)
{
    DPTR_D(VideoDecoderFFmpegHW);

    Q_ASSERT_X(src[0] && (pitch[0] > 0), "VideoDecoderFFmpegHW::copyToFrame", "src[0] and pitch[0] must be set");

    const int nb_planes    = fmt.planeCount();
    const int chroma_pitch = (nb_planes > 1) ? fmt.bytesPerLine(pitch[0], 1) : 0;
    const int chroma_h     = fmt.chromaHeight(surface_h);

    int h[] =
    {
        surface_h,
        0,
        0
    };

    for (int i = 1 ; i < nb_planes ; ++i)
    {
        h[i] = chroma_h;

        // set chroma address and pitch if not set

        if (pitch[i] <= 0)
            pitch[i] = chroma_pitch;

        if (!src[i])
            src[i] = src[i-1] + pitch[i-1] * h[i-1];
    }

    if (swapUV && (nb_planes > 2))
    {
        std::swap(src[1],   src[2]);
        std::swap(pitch[1], pitch[2]);
    }

    VideoFrame frame;

    if ((copyMode() == VideoDecoderFFmpegHW::OptimizedCopy) && d.gpu_mem.isReady())
    {
        int yuv_size = 0;

        for (int i = 0 ; i < nb_planes ; ++i)
        {
            yuv_size += pitch[i]*h[i];
        }

        // additional 15 bytes to ensure 16 bytes aligned

        QByteArray buf(15 + yuv_size, 0);
        const int offset_16 = (16 - ((uintptr_t)buf.data() & 0x0f)) & 0x0f;

        // plane 1, 2... is aligned?

        uchar* plane_ptr    = (uchar*)buf.data() + offset_16;
        QVector<uchar*> dst(nb_planes, 0);

        for (int i = 0 ; i < nb_planes ; ++i)
        {
            dst[i]     = plane_ptr;

            // TODO: add VideoFormat::planeWidth/Height() ?

            // pitch instead of surface_width

            plane_ptr += pitch[i] * h[i];
            d.gpu_mem.copyFrame(src[i], dst[i], pitch[i], h[i], pitch[i]);
        }

        frame = VideoFrame(d.width, d.height, fmt, buf);
        frame.setBits(dst);
        frame.setBytesPerLine(pitch);
    }
    else
    {
        frame = VideoFrame(d.width, d.height, fmt);
        frame.setBits(src);
        frame.setBytesPerLine(pitch);

        // TODO: why clone is faster()?
        // TODO: buffer pool and create VideoFrame when needed to avoid copy? also for other va

        frame = frame.clone();
    }

#ifndef HAVE_FFMPEG_VERSION5

    frame.setTimestamp(double(d.frame->pkt_pts) / 1000.0);

#else // ffmpeg >= 5

    frame.setTimestamp(double(d.frame->pts) / 1000.0);

#endif

    frame.setDisplayAspectRatio(d.getDAR(d.frame));
    d.updateColorDetails(&frame);

    return frame;
}

} // namespace QtAV
