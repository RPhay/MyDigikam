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

#include "VideoDecoderFFmpegBase.h"

// Local includes

#include "Packet.h"
#include "digikam_debug.h"

namespace QtAV
{

extern ColorSpace colorSpaceFromFFmpeg(AVColorSpace cs);
extern ColorRange colorRangeFromFFmpeg(AVColorRange cr);

static void SetColorDetailsByFFmpeg(VideoFrame* f, AVFrame* frame, AVCodecContext* codec_ctx)
{

#ifndef HAVE_FFMPEG_VERSION5

    ColorSpace cs = colorSpaceFromFFmpeg(av_frame_get_colorspace(frame));

    if (cs == ColorSpace_Unknown)
        cs = colorSpaceFromFFmpeg(codec_ctx->colorspace);

#else // ffmpeg >= 5

     ColorSpace cs = colorSpaceFromFFmpeg(codec_ctx->colorspace);

#endif

    f->setColorSpace(cs);

#ifndef HAVE_FFMPEG_VERSION5

    ColorRange cr = colorRangeFromFFmpeg(av_frame_get_color_range(frame));

    if (cr == ColorRange_Unknown)

#else // ffmpeg >= 5

    ColorRange cr = ColorRange_Unknown;

#endif

    {
        // check yuvj format. TODO: deprecated, check only for old ffmpeg?

        const AVPixelFormat pixfmt = (AVPixelFormat)frame->format;

        switch (pixfmt)
        {
/*
            case QTAV_PIX_FMT_C(YUVJ411P): // not in ffmpeg < 2 and libav
*/
            case QTAV_PIX_FMT_C(YUVJ420P):
            case QTAV_PIX_FMT_C(YUVJ422P):
            case QTAV_PIX_FMT_C(YUVJ440P):
            case QTAV_PIX_FMT_C(YUVJ444P):
            {
                cr = ColorRange_Full;

                break;
            }

            default:
            {
                break;
            }
        }
    }

    if (cr == ColorRange_Unknown)
    {
        cr = colorRangeFromFFmpeg(codec_ctx->color_range);

        if (cr == ColorRange_Unknown)
        {
            if      (f->format().isXYZ())
            {
                cr = ColorRange_Full;
                cs = ColorSpace_XYZ; // not here

                Q_UNUSED(cs);
            }
            else if (!f->format().isRGB())
            {
/*
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("prefer limited yuv range");
*/
                cr = ColorRange_Limited;
            }
        }
    }

    f->setColorRange(cr);
}

void VideoDecoderFFmpegBasePrivate::updateColorDetails(VideoFrame* f)
{
    if (f->format().pixelFormatFFmpeg() == frame->format)
    {
        SetColorDetailsByFFmpeg(f, frame, codec_ctx);

        return;
    }

    // hw decoder output frame may have a different format, e.g. gl interop frame
    // may have rgb format for rendering(stored as yuv)

    const bool rgb_frame = f->format().isRGB();

    if (rgb_frame)
    {
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("rgb output frame (yuv coded)");
*/
        f->setColorSpace(f->format().isPlanar() ? ColorSpace_GBR : ColorSpace_RGB);
        f->setColorRange(ColorRange_Full);

        return;
    }

    // yuv frame. When happens?

    const bool rgb_coded = ((av_pix_fmt_desc_get(codec_ctx->pix_fmt)->flags & AV_PIX_FMT_FLAG_RGB) == AV_PIX_FMT_FLAG_RGB);

    if (rgb_coded)
    {
        if ((f->width() >= 1280) && (f->height() >= 576))
            f->setColorSpace(ColorSpace_BT709);
        else
            f->setColorSpace(ColorSpace_BT601);

        f->setColorRange(ColorRange_Limited);
    }
    else
    {
        SetColorDetailsByFFmpeg(f, frame, codec_ctx);
    }
}

qreal VideoDecoderFFmpegBasePrivate::getDAR(AVFrame* f)
{
    // lavf 54.5.100 av_guess_sample_aspect_ratio: stream.sar > frame.sar

    qreal dar = 0.0;

    if (f->height > 0)
        dar = (qreal)f->width / (qreal)f->height;

    // prefer sar from AVFrame if sar != 1/1

    if      (f->sample_aspect_ratio.num > 1)
        dar *= av_q2d(f->sample_aspect_ratio);
    else if (codec_ctx && (codec_ctx->sample_aspect_ratio.num > 1)) // skip 1/1
        dar *= av_q2d(codec_ctx->sample_aspect_ratio);

    return dar;
}

VideoDecoderFFmpegBase::VideoDecoderFFmpegBase(VideoDecoderFFmpegBasePrivate& d)
    : VideoDecoder(d)
{
}

bool VideoDecoderFFmpegBase::decode(const Packet& packet)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoDecoderFFmpegBase);

#ifndef HAVE_FFMPEG_VERSION5
    // some decoders might need other fields like flags&AV_PKT_FLAG_KEY
    // const AVPacket*: ffmpeg >= 1.0. no libav

    int got_frame_ptr = 0;
    int ret           = 0;

    if (packet.isEOF())
    {
        AVPacket eofpkt;
        av_init_packet(&eofpkt);
        eofpkt.data = nullptr;
        eofpkt.size = 0;

        ret         = avcodec_decode_video2(d.codec_ctx,
                                            d.frame,
                                            &got_frame_ptr,
                                            &eofpkt);
    }
    else
    {
        ret = avcodec_decode_video2(d.codec_ctx,
                                    d.frame,
                                    &got_frame_ptr,
                                    const_cast<AVPacket*>(packet.asAVPacket()));
    }
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("pic_type=%c", av_get_picture_type_char(d.frame->pict_type));
*/
    d.undecoded_size = qMin(packet.data.size() - ret, packet.data.size());

    if (ret < 0)
    {
/*
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("[VideoDecoderFFmpegBase] %s", av_err2str(ret));
*/
        return false;
    }

    if (!got_frame_ptr)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN) << "no frame could be decompressed:"
                                         << av_err2str(ret)
                                         << d.undecoded_size
                                         << "/" << packet.data.size();

        return !packet.isEOF();
    }

    if (!d.codec_ctx->width || !d.codec_ctx->height)
        return false;
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("codec %dx%d, frame %dx%d",
            d.codec_ctx->width, d.codec_ctx->height, d.frame->width, d.frame->height);
*/

#else // ffmpeg >= 5

    int ret;
    d.undecoded_size = 0; // code below always consumes entire packet

    if (packet.isEOF())
    {
        AVPacket eofpkt;

        if (av_new_packet(&eofpkt, 0) < 0)
        {
            return false;
        }

        eofpkt.data = nullptr;
        eofpkt.size = 0;
        ret = avcodec_send_packet(d.codec_ctx, &eofpkt);
    }
    else
    {
        ret = avcodec_send_packet(d.codec_ctx, packet.asAVPacket());
    }

    if ((ret < 0) && (ret != AVERROR_EOF) && (ret != AVERROR(EAGAIN)))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("[VideoDecoder] %s",
                av_err2str(ret));
        return false;
    }

    if (ret == AVERROR(EAGAIN))
    {
        d.undecoded_size = packet.data.size();
    }

    ret = avcodec_receive_frame(d.codec_ctx, d.frame);

    if ((ret < 0) && (ret != AVERROR(EAGAIN)))
    {
        return false;
    }

#endif

    d.width  = d.frame->width; // TODO: remove? used in hwdec
    d.height = d.frame->height;
/*
    avcodec_align_dimensions2(d.codec_ctx, &d.width_align, &d.height_align, aligns);
*/
    return true;
}

VideoFrame VideoDecoderFFmpegBase::frame()
{
    DPTR_D(VideoDecoderFFmpegBase);

    if ((d.frame->width <= 0) || (d.frame->height <= 0) || !d.codec_ctx)
        return VideoFrame();

    // it's safe if width, height, pixfmt will not change, only data change

    VideoFrame frame(d.frame->width, d.frame->height, VideoFormat((int)d.codec_ctx->pix_fmt));
    frame.setDisplayAspectRatio(d.getDAR(d.frame));
    frame.setBits(d.frame->data);
    frame.setBytesPerLine(d.frame->linesize);

    // in s. TODO: what about AVFrame.pts? av_frame_get_best_effort_timestamp? move to VideoFrame::from(AVFrame*)

#ifndef HAVE_FFMPEG_VERSION5

    frame.setTimestamp((double)d.frame->pkt_pts / 1000.0);

#else // ffmpeg >= 5

    frame.setTimestamp((double)d.frame->pts / 1000.0);

#endif

    frame.setMetaData(QLatin1String("avbuf"), QVariant::fromValue(AVFrameBuffersRef(new AVFrameBuffers(d.frame))));
    d.updateColorDetails(&frame);

    if (frame.format().hasPalette())
    {
        frame.setMetaData(QLatin1String("pallete"), QByteArray((const char*)d.frame->data[1], 256 * 4));
    }

    return frame;
}

} // namespace QtAV
