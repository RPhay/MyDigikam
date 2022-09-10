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

//#include <OpenGLES/EAGL.h>

// C++ includes

#include <assert.h>

// FFMpeg includes

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include <libavcodec/videotoolbox.h>

#ifdef __cplusplus

}

#endif //__cplusplus

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "GPUMemCopy.h"
#include "AVCompat.h"
#include "QtAV_factory.h"
#include "SurfaceInteropCV.h"
#include "digikam_debug.h"

#ifdef MAC_OS_X_VERSION_MIN_REQUIRED
#   if MAC_OS_X_VERSION_MIN_REQUIRED >= 1070 //MAC_OS_X_VERSION_10_7
#       define OSX_TARGET_MIN_LION
#   endif // 1070
#endif // MAC_OS_X_VERSION_MIN_REQUIRED

#ifndef kCFCoreFoundationVersionNumber10_7
#   define kCFCoreFoundationVersionNumber10_7      635.00
#endif

//kCVPixelBufferOpenGLESCompatibilityKey //ios6

namespace QtAV
{

class VideoDecoderVideoToolboxPrivate;

// qt4 moc can not correctly process final here

class Q_DECL_HIDDEN VideoDecoderVideoToolbox : public VideoDecoderFFmpegHW
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(VideoDecoderVideoToolbox)
    Q_PROPERTY(PixelFormat format READ format WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(Interop interop READ interop WRITE setInterop NOTIFY interopChanged)

    // TODO: try async property

public:

    enum PixelFormat
    {
        NV12    = '420v',
        UYVY    = '2vuy',
        BGRA    = 'BGRA', ///< kCVPixelFormatType_32BGRA
        YUV420P = 'y420',
        YUYV    = 'yuvs'
    };
    Q_ENUM(PixelFormat)

    // no ios macro check in Interop because moc is an idiot

    enum Interop
    {
        CVPixelBuffer = cv::InteropCVPixelBuffer,
        CVOpenGLES    = cv::InteropCVOpenGLES,
        IOSurface     = cv::InteropIOSurface,
        Auto          = cv::InteropAuto
    };
    Q_ENUM(Interop)

    VideoDecoderVideoToolbox();

    VideoDecoderId id()   const override;
    QString description() const override;
    VideoFrame frame()          override;

    // QObject properties

    void setFormat(PixelFormat fmt);
    PixelFormat format()  const;

    void setInterop(Interop value);
    Interop interop()     const;

Q_SIGNALS:

    void formatChanged();
    void interopChanged();
};

extern VideoDecoderId VideoDecoderId_VideoToolbox;

FACTORY_REGISTER(VideoDecoder, VideoToolbox, "VideoToolbox")

class Q_DECL_HIDDEN VideoDecoderVideoToolboxPrivate final : public VideoDecoderFFmpegHWPrivate
{
public:

    VideoDecoderVideoToolboxPrivate()
        : VideoDecoderFFmpegHWPrivate(),
          out_fmt     (VideoDecoderVideoToolbox::NV12),
          interop_type(cv::InteropAuto)
    {
        if (kCFCoreFoundationVersionNumber < kCFCoreFoundationVersionNumber10_7)
            out_fmt = VideoDecoderVideoToolbox::UYVY;

        copy_mode   = VideoDecoderFFmpegHW::ZeroCopy;
        description = QLatin1String("VideoToolbox");
    }

    ~VideoDecoderVideoToolboxPrivate()
    {
    }

    bool open()                                     override;
    void close()                                    override;

    void* setup(AVCodecContext* avctx)              override;
    bool getBuffer(void** opaque, uint8_t** data)   override;
    void releaseBuffer(void* opaque, uint8_t* data) override;

    AVPixelFormat vaPixelFormat()             const override
    {
        return AV_PIX_FMT_VIDEOTOOLBOX;
    }

    VideoDecoderVideoToolbox::PixelFormat out_fmt;
    cv::InteropType                       interop_type;
    cv::InteropResourcePtr                interop_res;
};

typedef struct Q_DECL_HIDDEN
{
    int         code = 0;
    const char* str  = nullptr;
} cv_error;

static const cv_error cv_errors[] =
{
    {
        AVERROR(ENOSYS),
            "Hardware doesn't support accelerated decoding for this stream"
            " or Videotoolbox decoder is not available at the moment (another"
            " application is using it)."
    },
    {
        AVERROR(EINVAL),
            "Invalid configuration provided to VTDecompressionSessionCreate"
    },
    {
        AVERROR_INVALIDDATA,
            "Generic error returned by the decoder layer. The cause can be Videotoolbox"
            " found errors in the bitstream."
    },
    {
        0,
        nullptr
    },
};

static const char* cv_err_str(int err)
{
    for (int i = 0 ; cv_errors[i].code ; ++i)
    {
        if (cv_errors[i].code != err)
            continue;

        return cv_errors[i].str;
    }

    return "Unknown error";
}

VideoDecoderVideoToolbox::VideoDecoderVideoToolbox()
    : VideoDecoderFFmpegHW(*new VideoDecoderVideoToolboxPrivate())
{

#if 1   // !AV_MODULE_CHECK(LIBAVCODEC, 57, 30, 1, 89, 100) // ffmpeg3.3

    setProperty("threads", 1); // to avoid crash at av_videotoolbox_alloc_context/av_videotoolbox_default_free. I have no idea how the are called

#endif

    // dynamic properties about static property details. used by UI

    setProperty("detail_format", i18n("Output pixel format from decoder. "
                                      "Performance (better first): NV12 ; UYVY ; BGRA ; YUV420P ; YUYV. "
                                      "OSX superior to 10.7 only supports UYVY, BGRA and YUV420p"));
    setProperty("detail_interop",
                QString::fromLatin1("%1\n%2\n%3\n%4\n%5")
                    .arg(i18n("Interop with OpenGL"))
                    .arg(i18n("CVPixelBuffer: macOS+iOS"))
                    .arg(i18n("CVOpenGLES: iOS, no copy, fast"))
                    .arg(i18n("IOSurface: macOS, no copy, fast"))
                    .arg(i18n("Auto: choose the fastest")));
}

VideoDecoderId VideoDecoderVideoToolbox::id() const
{
    return VideoDecoderId_VideoToolbox;
}

QString VideoDecoderVideoToolbox::description() const
{
    return QLatin1String("Apple VideoToolbox");
}

VideoFrame VideoDecoderVideoToolbox::frame()
{
    DPTR_D(VideoDecoderVideoToolbox);

    if (!d.codec_ctx->hwaccel_context)
    {
        return VideoDecoderFFmpegBase::frame();
    }

    CVPixelBufferRef cv_buffer = (CVPixelBufferRef)d.frame->data[3];

    if (!cv_buffer)
    {
        qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("Frame buffer is empty.");

        return VideoFrame();
    }

    if (CVPixelBufferGetDataSize(cv_buffer) <= 0)
    {
        qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("Empty frame buffer");

        return VideoFrame();
    }

    const VideoFormat::PixelFormat pixfmt = cv::format_from_cv(CVPixelBufferGetPixelFormatType(cv_buffer));

    if (pixfmt == VideoFormat::Format_Invalid)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("unsupported cv pixel format: %#x",
                (quint32)CVPixelBufferGetPixelFormatType(cv_buffer));

        return VideoFrame();
    }

    uint8_t* src[3]      = { nullptr };
    int pitch[3]; // must get the value from cvbuffer to compute opengl text size in VideoShader
    VideoFormat fmt(pixfmt);
    const bool zero_copy = (copyMode() == ZeroCopy);
    CVPixelBufferLockBaseAddress(cv_buffer, kCVPixelBufferLock_ReadOnly);

    for (int i = 0 ; i < fmt.planeCount() ; ++i)
    {
        pitch[i] = CVPixelBufferGetBytesPerRowOfPlane(cv_buffer, i);

        // get address results in internal copy

        if (!zero_copy)
            src[i] = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(cv_buffer, i);
    }

    CVPixelBufferUnlockBaseAddress(cv_buffer, kCVPixelBufferLock_ReadOnly);

    //CVPixelBufferRelease(cv_buffer); // release when video frame is destroyed

    VideoFrame f;

    if (zero_copy)
    {
        if (d.interop_res)
        {
            // make sure VideoMaterial can correctly setup parameters

            VideoFormat::PixelFormat opixfmt = pixfmt;
            d.interop_res->stridesForWidth(format(), d.width, pitch, &opixfmt);

            if (pixfmt != opixfmt)
                fmt = VideoFormat(opixfmt);
        }

        f = VideoFrame(d.width, d.height, fmt);
        f.setBytesPerLine(pitch);

        // TODO: move to updateFrameInfo

#ifndef HAVE_FFMPEG_VERSION5

        f.setTimestamp((double)d.frame->pkt_pts / 1000.0);

#else // ffmpeg >= 5

         f.setTimestamp((double)d.frame->pts / 1000.0);

#endif

        f.setDisplayAspectRatio(d.getDAR(d.frame));
        d.updateColorDetails(&f);

        if (d.interop_res)
        {
            // zero_copy

            cv::SurfaceInteropCV* const interop = new cv::SurfaceInteropCV(d.interop_res);
            interop->setSurface(cv_buffer, d.width, d.height);
            f.setMetaData(QLatin1String("surface_interop"), QVariant::fromValue(VideoSurfaceInteropPtr(interop)));

            if (!d.interop_res->mapToTexture2D())
                f.setMetaData(QLatin1String("target"), QByteArray("rect"));
        }
        else
        {
            f.setBits(src); // only set for copy back mode
        }
    }
    else
    {
        f = copyToFrame(fmt, d.height, src, pitch, false);
    }

    return f;
}

void VideoDecoderVideoToolbox::setFormat(PixelFormat fmt)
{
    DPTR_D(VideoDecoderVideoToolbox);

    if (d.out_fmt == fmt)
        return;

    d.out_fmt = fmt;

    Q_EMIT formatChanged();

    if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber10_7)
        return;

    if ((fmt != YUV420P) && (fmt != UYVY))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("format is not supported on OSX < 10.7");
    }
}

VideoDecoderVideoToolbox::PixelFormat VideoDecoderVideoToolbox::format() const
{
    return d_func().out_fmt;
}

void VideoDecoderVideoToolbox::setInterop(Interop value)
{
    DPTR_D(VideoDecoderVideoToolbox);

    if (value == (Interop)d.interop_type)
        return;

    d.interop_type = (cv::InteropType)value;

    Q_EMIT interopChanged();
}

VideoDecoderVideoToolbox::Interop VideoDecoderVideoToolbox::interop() const
{
    return (Interop)d_func().interop_type;
}

void* VideoDecoderVideoToolboxPrivate::setup(AVCodecContext* avctx)
{
    releaseUSWC();
    av_videotoolbox_default_free(avctx);
    AVVideotoolboxContext* const vtctx = av_videotoolbox_alloc_context();
    vtctx->cv_pix_fmt_type             = out_fmt;

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("AVVideotoolboxContext: %p", vtctx);

    int err = av_videotoolbox_default_init2(avctx, vtctx); // ios h264 crashes when processing extra data. null H264Context

    if (err < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Failed to init videotoolbox decoder (%#x %s): %s",
                err, av_err2str(err), cv_err_str(err));

        return nullptr;
    }

    const CMVideoDimensions dim = CMVideoFormatDescriptionGetDimensions(vtctx->cm_fmt_desc);
    initUSWC(codedWidth(avctx)); // TODO: use stride

    qCDebug(DIGIKAM_QTAV_LOG)
        << "VideoToolbox decoder created. format: "
            << cv::format_from_cv(out_fmt);

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("AVVideotoolboxContext ready: %dx%d",
            dim.width, dim.height);

    return vtctx; // the same as avctx->hwaccel_context;
}

bool VideoDecoderVideoToolboxPrivate::getBuffer(void** opaque, uint8_t** data)
{
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("vt getbuffer");

    *data = (uint8_t*)1; // dummy. it's AVFrame.data[0], must be non null required by ffmpeg
    Q_UNUSED(opaque);

    return true;
}

void VideoDecoderVideoToolboxPrivate::releaseBuffer(void* opaque, uint8_t* data)
{
    Q_UNUSED(opaque);
    Q_UNUSED(data);
}

bool VideoDecoderVideoToolboxPrivate::open()
{
    if (!prepare())
        return false;

    switch (codec_ctx->profile)
    {
        // profile check code is from xbmc

        case FF_PROFILE_H264_HIGH_10: // Apple A7 SoC
        case FF_PROFILE_H264_HIGH_10_INTRA:
        case FF_PROFILE_H264_HIGH_422:
        case FF_PROFILE_H264_HIGH_422_INTRA:
        case FF_PROFILE_H264_HIGH_444_PREDICTIVE:
        case FF_PROFILE_H264_HIGH_444_INTRA:
        case FF_PROFILE_H264_CAVLC_444:
        {
            return false;
        }

        default:
        {
            break;
        }
    }

    switch (codec_ctx->codec_id)
    {
        case AV_CODEC_ID_HEVC:
        case AV_CODEC_ID_H264:
        case AV_CODEC_ID_H263:
        case AV_CODEC_ID_MPEG4:
        case AV_CODEC_ID_MPEG2VIDEO:
        case AV_CODEC_ID_MPEG1VIDEO:
        {
            break;
        }

        default:
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("VideoToolbox unsupported codec: %s",
                    avcodec_get_name(codec_ctx->codec_id));

            return false;
        }
    }

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("opening VideoToolbox module");

    // setup() must be called in getFormat() from avcodec callback, otherwise in ffmpeg3.0 avctx->priv_data is null and crash
    // TODO: block AVDecoder.open() until hw callback is done
/*
    if (!setup(codec_ctx))
        return false;
*/
    if (copy_mode == VideoDecoderFFmpegHW::ZeroCopy)
    {
        interop_res = cv::InteropResourcePtr(cv::InteropResource::create(interop_type));
    }
    else
    {
        interop_res.clear();
    }

    return true;
}

void VideoDecoderVideoToolboxPrivate::close()
{
    restore(); // IMPORTANT. can not call restore in dtor because ctx is 0 there

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("destroying VideoToolbox decoder");

    if (codec_ctx)
    {
        av_videotoolbox_default_free(codec_ctx);
    }

    releaseUSWC();
}

} // namespace QtAV

#include "VideoDecoderVideoToolbox.moc"
