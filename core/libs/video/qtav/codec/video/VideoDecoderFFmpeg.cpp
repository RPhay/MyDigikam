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

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "QtAV_Version.h"
#include "AVCompat.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

/*!
 * options (properties) are from libavcodec/options_table.h
 * enum name here must convert to lower case to fit the names in avcodec. done in AVDecoder.setOptions()
 * Don't use lower case here because the value name may be "default" in avcodec which is a keyword of C++
 */

namespace QtAV
{

class VideoDecoderFFmpegPrivate;

class Q_DECL_HIDDEN VideoDecoderFFmpeg : public VideoDecoderFFmpegBase
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(VideoDecoderFFmpeg)
    Q_PROPERTY(QString codecName READ codecName WRITE setCodecName NOTIFY codecNameChanged)
    Q_PROPERTY(QString hwaccel READ hwaccel WRITE setHwaccel NOTIFY hwaccelChanged)
    Q_PROPERTY(DiscardType skip_loop_filter READ skipLoopFilter WRITE setSkipLoopFilter)
    Q_PROPERTY(DiscardType skip_idct READ skipIDCT WRITE setSkipIDCT)

    // Force a strict standard compliance when encoding (accepted values: -2 to 2)

    //Q_PROPERTY(StrictType strict READ strict WRITE setStrict)

    Q_PROPERTY(DiscardType skip_frame READ skipFrame WRITE setSkipFrame)
    Q_PROPERTY(int threads READ threads WRITE setThreads) // 0 is auto
    Q_PROPERTY(ThreadFlags thread_type READ threadFlags WRITE setThreadFlags)
    Q_PROPERTY(MotionVectorVisFlags vismv READ motionVectorVisFlags WRITE setMotionVectorVisFlags)

    //Q_PROPERTY(BugFlags bug READ bugFlags WRITE setBugFlags)

    Q_FLAGS(ThreadFlags)
    Q_FLAGS(MotionVectorVisFlags)
    Q_FLAGS(BugFlags)

public:

    enum StrictType
    {
        Very            = FF_COMPLIANCE_VERY_STRICT,
        Strict          = FF_COMPLIANCE_STRICT,
        Normal          = FF_COMPLIANCE_NORMAL, // default
        Unofficial      = FF_COMPLIANCE_UNOFFICIAL,
        Experimental    = FF_COMPLIANCE_EXPERIMENTAL
    };
    Q_ENUM(StrictType)

    enum DiscardType
    {
        // TODO: discard_type

        None    = AVDISCARD_NONE,
        Default = AVDISCARD_DEFAULT, // default
        NoRef   = AVDISCARD_NONREF,
        Bidir   = AVDISCARD_BIDIR,
        NoKey   = AVDISCARD_NONKEY,
        All     = AVDISCARD_ALL
    };
    Q_ENUM(DiscardType)

    enum ThreadFlag
    {
        DefaultType = FF_THREAD_SLICE | FF_THREAD_FRAME, // default
        Slice       = FF_THREAD_SLICE,
        Frame       = FF_THREAD_FRAME
    };
    Q_DECLARE_FLAGS(ThreadFlags, ThreadFlag)
    Q_ENUM(ThreadFlag)

    // flags. visualize motion vectors (MVs)

    enum MotionVectorVisFlag
    {
        No = 0 // default
#   if LIBAVCODEC_VERSION_MAJOR < 60
        ,
        PF = FF_DEBUG_VIS_MV_P_FOR,
        BF = FF_DEBUG_VIS_MV_B_FOR,
        BB = FF_DEBUG_VIS_MV_B_BACK
#   endif
    };
    Q_DECLARE_FLAGS(MotionVectorVisFlags, MotionVectorVisFlag)
    Q_ENUM(MotionVectorVisFlag)

    enum BugFlag
    {
        autodetect          = FF_BUG_AUTODETECT,  // default

#if FF_API_OLD_MSMPEG4
/*
        old_msmpeg4         = FF_BUG_OLD_MSMPEG4, // moc does not support PP?
*/
#endif

        xvid_ilace          = FF_BUG_XVID_ILACE,
        ump4                = FF_BUG_UMP4,
        no_padding          = FF_BUG_NO_PADDING,
        amv                 = FF_BUG_AMV,

#if FF_API_AC_VLC

        //ac_vlc = FF_BUG_AC_VLC, //moc does not support PP?

#endif

        qpel_chroma         = FF_BUG_QPEL_CHROMA,
        std_qpel            = FF_BUG_QPEL_CHROMA2,
        direct_blocksize    = FF_BUG_DIRECT_BLOCKSIZE,
        edge                = FF_BUG_EDGE,
        hpel_chroma         = FF_BUG_HPEL_CHROMA,
        dc_clip             = FF_BUG_DC_CLIP,
        ms                  = FF_BUG_MS,
        trunc               = FF_BUG_TRUNCATED
    };
    Q_DECLARE_FLAGS(BugFlags, BugFlag)
    Q_ENUM(BugFlag)

    static VideoDecoder* createMMAL()
    {
        VideoDecoderFFmpeg* const vd = new VideoDecoderFFmpeg();
        vd->setProperty("hwaccel", QLatin1String("mmal"));

        return vd;
    }

    static VideoDecoder* createQSV()
    {
        VideoDecoderFFmpeg* const vd = new VideoDecoderFFmpeg();
        vd->setProperty("hwaccel", QLatin1String("qsv"));

        return vd;
    }

    static VideoDecoder* createCrystalHD()
    {
        VideoDecoderFFmpeg* const vd = new VideoDecoderFFmpeg();
        vd->setProperty("hwaccel", QLatin1String("crystalhd"));

        return vd;
    }

    static void registerHWA()
    {

#if defined(Q_OS_WIN32) || (defined(Q_OS_LINUX) && !defined(Q_PROCESSOR_ARM) && !defined(QT_ARCH_ARM))

        VideoDecoder::Register(VideoDecoderId_QSV,       createQSV,       "QSV");

#endif

#ifdef Q_OS_LINUX
#   if defined(Q_PROCESSOR_ARM)/*qt5*/ || defined(QT_ARCH_ARM) /*qt4*/

        VideoDecoder::Register(VideoDecoderId_MMAL,      createMMAL,      "MMAL");

#   else

        VideoDecoder::Register(VideoDecoderId_CrystalHD, createCrystalHD, "CrystalHD");

#   endif
#endif

    }

    VideoDecoderFFmpeg();
    VideoDecoderId id()                         const override final;

    QString description()                       const override final
    {
        const int patch = QTAV_VERSION_PATCH(avcodec_version());

        return QString::fromUtf8("%1 avcodec %2.%3.%4")
                .arg((patch >= 100) ? QLatin1String("FFmpeg") : QLatin1String("Libav"))
                .arg(QTAV_VERSION_MAJOR(avcodec_version())).arg(QTAV_VERSION_MINOR(avcodec_version())).arg(patch);
    }

    // TODO: av_opt_set in setter

    void setSkipLoopFilter(DiscardType value);
    DiscardType skipLoopFilter()                const;

    void setSkipIDCT(DiscardType value);
    DiscardType skipIDCT()                      const;

    void setStrict(StrictType value);
    StrictType strict()                         const;

    void setSkipFrame(DiscardType value);
    DiscardType skipFrame()                     const;

    void setThreads(int value);
    int threads()                               const;

    void setThreadFlags(ThreadFlags value);
    ThreadFlags threadFlags()                   const;

    void setMotionVectorVisFlags(MotionVectorVisFlags value);
    MotionVectorVisFlags motionVectorVisFlags() const;

    void setBugFlags(BugFlags value);
    BugFlags bugFlags()                         const;

    void setHwaccel(const QString& value);
    QString hwaccel()                           const;

Q_SIGNALS:

    void hwaccelChanged();

private:

    // Disable

    VideoDecoderFFmpeg(QObject*);
};

extern VideoDecoderId VideoDecoderId_FFmpeg;

FACTORY_REGISTER(VideoDecoder, FFmpeg, "FFmpeg")

void RegisterFFmpegHWA_Man()
{
    VideoDecoderFFmpeg::registerHWA();
}

namespace
{
    static const struct Q_DECL_HIDDEN factory_register_FFmpegHWA
    {
        inline factory_register_FFmpegHWA()
        {
            VideoDecoderFFmpeg::registerHWA();
        }
    } sInit_FFmpegHWA;
}

class Q_DECL_HIDDEN VideoDecoderFFmpegPrivate final : public VideoDecoderFFmpegBasePrivate
{
public:

    VideoDecoderFFmpegPrivate()
      : VideoDecoderFFmpegBasePrivate(),
        skip_loop_filter(VideoDecoderFFmpeg::Default),
        skip_idct       (VideoDecoderFFmpeg::Default),
        strict          (VideoDecoderFFmpeg::Normal),
        skip_frame      (VideoDecoderFFmpeg::Default),
        thread_type     (VideoDecoderFFmpeg::DefaultType),
        threads         (0),
        debug_mv        (VideoDecoderFFmpeg::No),
        bug             (VideoDecoderFFmpeg::autodetect)
    {
    }

    bool open() override
    {
        av_opt_set_int(codec_ctx, "skip_loop_filter", (int64_t)skip_loop_filter,    0);
        av_opt_set_int(codec_ctx, "skip_idct",        (int64_t)skip_idct,           0);
        av_opt_set_int(codec_ctx, "strict",           (int64_t)strict,              0);
        av_opt_set_int(codec_ctx, "skip_frame",       (int64_t)skip_frame,          0);
        av_opt_set_int(codec_ctx, "threads",          (int64_t)threads,             0);
        av_opt_set_int(codec_ctx, "thread_type",      (int64_t)thread_type,         0);
        av_opt_set_int(codec_ctx, "vismv",            (int64_t)debug_mv,            0);
        av_opt_set_int(codec_ctx, "bug",              (int64_t)bug,                 0);

        //CODEC_FLAG_EMU_EDGE: deprecated in ffmpeg >=? & libav>=10. always set by ffmpeg

#if 0

        if (fast)
        {
            codec_ctx->flags2 |= CODEC_FLAG2_FAST; // TODO:
        }
        else
        {
/*
            codec_ctx->flags2 &= ~CODEC_FLAG2_FAST; // ffplay has no this
*/
        }

        // lavfilter
/*
        codec_ctx->slice_flags |= SLICE_FLAG_ALLOW_FIELD; //lavfilter
        codec_ctx->strict_std_compliance = FF_COMPLIANCE_STRICT;
*/
        codec_ctx->thread_safe_callbacks = true;

        switch (codec_ctx->codec_id)
        {
            case QTAV_CODEC_ID(MPEG4):
            case QTAV_CODEC_ID(H263):
            {
                codec_ctx->thread_type = 0;

                break;
            }

            case QTAV_CODEC_ID(MPEG1VIDEO):
            case QTAV_CODEC_ID(MPEG2VIDEO):
            {
                codec_ctx->thread_type &= ~FF_THREAD_SLICE;

                break;
            }

            /* fall through */

#   if (LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 1, 0))

            case QTAV_CODEC_ID(H264):
            case QTAV_CODEC_ID(VC1):
            case QTAV_CODEC_ID(WMV3):
            {
                codec_ctx->thread_type &= ~FF_THREAD_FRAME;

                break;
            }

#   endif

            default:
            {
                break;
            }
        }

#endif

        return true;
    }

public:

    int     skip_loop_filter;
    int     skip_idct;
    int     strict;
    int     skip_frame;
    int     thread_type;
    int     threads;
    int     debug_mv;
    int     bug;
    QString hwa;
};

VideoDecoderFFmpeg::VideoDecoderFFmpeg()
    : VideoDecoderFFmpegBase(*new VideoDecoderFFmpegPrivate())
{
    // dynamic properties about static property details. used by UI
    // format: detail_property

    setProperty("detail_skip_loop_filter", i18n("Skipping the loop filter (aka deblocking) usually "
                                                "has determinal effect on quality. However it provides "
                                                "a big speedup for hi definition streams"));

    // like skip_frame

    setProperty("detail_skip_idct", i18n("Force skipping of idct to speed up decoding for frame types (-1=None, "
                                         "0=Default, 1=B-frames, 2=P-frames, 3=B+P frames, 4=all frames)"));
    setProperty("detail_skip_frame", i18n("Force skipping frames for speed up decoding."));
    setProperty("detail_threads", QString::fromUtf8("%1\n%2\n%3")
                .arg(i18n("Number of decoding threads. Set before open. Maybe no effect for some decoders"))
                .arg(i18n("0: auto"))
                .arg(i18n("1: single thread decoding")));
}

VideoDecoderId VideoDecoderFFmpeg::id() const
{
    DPTR_D(const VideoDecoderFFmpeg);

    if (d.hwa == QLatin1String("mmal"))
        return VideoDecoderId_MMAL;

    if (d.hwa == QLatin1String("qsv"))
        return VideoDecoderId_QSV;

    if (d.hwa == QLatin1String("crystalhd"))
        return VideoDecoderId_CrystalHD;

    return VideoDecoderId_FFmpeg;
}

void VideoDecoderFFmpeg::setSkipLoopFilter(DiscardType value)
{
    DPTR_D(VideoDecoderFFmpeg);
    d.skip_loop_filter = value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "skip_loop_filter", (int64_t)value, 0);
}

VideoDecoderFFmpeg::DiscardType VideoDecoderFFmpeg::skipLoopFilter() const
{
    return (DiscardType)d_func().skip_loop_filter;
}

void VideoDecoderFFmpeg::setSkipIDCT(DiscardType value)
{
    DPTR_D(VideoDecoderFFmpeg);
    d.skip_idct = (int)value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "skip_idct", (int64_t)value, 0);
}

VideoDecoderFFmpeg::DiscardType VideoDecoderFFmpeg::skipIDCT() const
{
    return (DiscardType)d_func().skip_idct;
}

void VideoDecoderFFmpeg::setStrict(StrictType value)
{
    DPTR_D(VideoDecoderFFmpeg);
    d.strict = (int)value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "strict", int64_t(value), 0);
}

VideoDecoderFFmpeg::StrictType VideoDecoderFFmpeg::strict() const
{
    return (StrictType)d_func().strict;
}

void VideoDecoderFFmpeg::setSkipFrame(DiscardType value)
{
    DPTR_D(VideoDecoderFFmpeg);
    d.skip_frame = (int)value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "skip_frame", (int64_t)value, 0);
}

VideoDecoderFFmpeg::DiscardType VideoDecoderFFmpeg::skipFrame() const
{
    return (DiscardType)d_func().skip_frame;
}

void VideoDecoderFFmpeg::setThreads(int value)
{
    DPTR_D(VideoDecoderFFmpeg);
    d.threads = value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "threads", (int64_t)value, 0);
}

int VideoDecoderFFmpeg::threads() const
{
    return d_func().threads;
}

void VideoDecoderFFmpeg::setThreadFlags(ThreadFlags value)
{
    DPTR_D(VideoDecoderFFmpeg);
    d.thread_type = (int)value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "thread_type", (int64_t)value, 0);
}

VideoDecoderFFmpeg::ThreadFlags VideoDecoderFFmpeg::threadFlags() const
{
    return (ThreadFlags)d_func().thread_type;
}

void VideoDecoderFFmpeg::setMotionVectorVisFlags(MotionVectorVisFlags value)
{
    DPTR_D(VideoDecoderFFmpeg);
    d.debug_mv = (int)value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "vismv", (int64_t)value, 0);
}

VideoDecoderFFmpeg::MotionVectorVisFlags VideoDecoderFFmpeg::motionVectorVisFlags() const
{
    return (MotionVectorVisFlags)d_func().debug_mv;
}

void VideoDecoderFFmpeg::setBugFlags(BugFlags value)
{
    DPTR_D(VideoDecoderFFmpeg);
    d.bug = (int)value;

    if (d.codec_ctx)
        av_opt_set_int(d.codec_ctx, "bug", (int64_t)value, 0);
}

VideoDecoderFFmpeg::BugFlags VideoDecoderFFmpeg::bugFlags() const
{
    return (BugFlags)d_func().bug;
}

void VideoDecoderFFmpeg::setHwaccel(const QString &value)
{
    DPTR_D(VideoDecoderFFmpeg);

    if (d.hwa == value)
        return;

    d.hwa = value.toLower();

    Q_EMIT hwaccelChanged();
}

QString VideoDecoderFFmpeg::hwaccel() const
{
    return d_func().hwa;
}

} // namespace QtAV

#include "VideoDecoderFFmpeg.moc"
