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

#include "QtAV_Global.h"

// Qt includes

#include <QLibraryInfo>
#include <QObject>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "QtAV_Version.h"
#include "AVCompat.h"
#include "QtAV_internal.h"
#include "digikam_debug.h"

unsigned QtAV_Version()
{
    return QTAV_VERSION;
}

QString QtAV_Version_String()
{
    return QString::fromLatin1(QTAV_VERSION_STR);
}

#define QTAV_VERSION_STR_LONG QTAV_VERSION_STR

QString QtAV_Version_String_Long()
{
    return QString::fromLatin1(QTAV_VERSION_STR_LONG);
}

namespace QtAV
{

namespace Internal
{

static int gAVLogLevel = AV_LOG_INFO;

} // namespace Internal

// TODO: auto add new depend libraries information

QString aboutFFmpeg_PlainText()
{

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

    return aboutFFmpeg_HTML().remove(QRegularExpression(QString::fromUtf8("<[^>]*>")));

#else

    return aboutFFmpeg_HTML().remove(QRegExp(QString::fromUtf8("<[^>]*>")));

#endif

}

namespace Internal
{

typedef struct Q_DECL_HIDDEN depend_component
{
    const char* lib           = nullptr;
    unsigned    build_version = 0;
    unsigned    rt_version    = 0;
    const char* config        = nullptr;
    const char* license       = nullptr;
} depend_component;

static unsigned get_qt_version()
{
    int major = 0;
    int minor = 0;
    int patch = 0;

    if (sscanf(qVersion(), "%d.%d.%d", &major, &minor, &patch) != 3)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Can not recognize Qt runtime version");
    }

    return QT_VERSION_CHECK(major, minor, patch);
}

static const depend_component* get_depend_component(const depend_component* info = nullptr)
{
    static const depend_component components[] =
    {
        {  "Qt", QT_VERSION, get_qt_version(), QLibraryInfo::build(), ""    },

        // TODO: auto check loaded libraries

#define FF_COMPONENT(name, NAME) #name,                   \
                                 LIB##NAME##_VERSION_INT, \
                                 name##_version(),        \
                                 name##_configuration(),  \
                                 name##_license()

        { FF_COMPONENT(avutil,     AVUTIL)                                  },
        { FF_COMPONENT(avcodec,    AVCODEC)                                 },
        { FF_COMPONENT(avformat,   AVFORMAT)                                },

#if QTAV_HAVE(AVFILTER)

        { FF_COMPONENT(avfilter,   AVFILTER)                                },

#endif

#if QTAV_HAVE(AVDEVICE)

        { FF_COMPONENT(avdevice,   AVDEVICE)                                },

#endif

#if QTAV_HAVE(AVRESAMPLE)

        { FF_COMPONENT(avresample, AVRESAMPLE)                              },

#endif

#if QTAV_HAVE(SWRESAMPLE)

        { FF_COMPONENT(swresample, SWRESAMPLE)                              },

#endif

        { FF_COMPONENT(swscale,    SWSCALE)                                 },

#undef FF_COMPONENT

        { nullptr, 0, 0, nullptr, nullptr                                   }
    };

    if (!info)
        return &components[0];

    // invalid input ptr

    if (((ptrdiff_t)info - (ptrdiff_t)(&components[0]))%sizeof(depend_component))
        return nullptr;

    const depend_component* next = info;
    next++;

    if (!next->lib)
        return nullptr;

    return next;
}

void print_library_info()
{
    qCDebug(DIGIKAM_QTAV_LOG) << aboutQtAV_PlainText().toUtf8().constData();

    const depend_component* info = Internal::get_depend_component(nullptr);

    while (info)
    {
        if (!qstrcmp(info->lib, "avutil"))
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("FFmpeg/Libav configuration: %s", info->config);
        }

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("Build with %s-%u.%u.%u",
               info->lib,
               QTAV_VERSION_MAJOR(info->build_version),
               QTAV_VERSION_MINOR(info->build_version),
               QTAV_VERSION_PATCH(info->build_version)
        );

        unsigned rt_version = info->rt_version;

        if (info->build_version != rt_version)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Warning: %s runtime version %u.%u.%u mismatch!",
                    info->lib,
                    QTAV_VERSION_MAJOR(rt_version),
                    QTAV_VERSION_MINOR(rt_version),
                    QTAV_VERSION_PATCH(rt_version)
            );
        }

        info = Internal::get_depend_component(info);
    }
}

} // namespace Internal

QString aboutFFmpeg_HTML()
{
    QString text                           = QLatin1String("<h3>FFmpeg/Libav</h3>\n");
    const Internal::depend_component* info = Internal::get_depend_component(nullptr);

    while (info)
    {
        text += QString::fromUtf8("<h4>%1: %2-%3.%4.%5</h4>\n")
                .arg(i18n("Build version"))
                .arg(QLatin1String(info->lib))
                .arg(QTAV_VERSION_MAJOR(info->build_version))
                .arg(QTAV_VERSION_MINOR(info->build_version))
                .arg(QTAV_VERSION_PATCH(info->build_version))
        ;

        unsigned rt_version = info->rt_version;

        if (info->build_version != rt_version)
        {
            text += QString::fromUtf8("<h4 style='color:#ff0000;'>%1: %2.%3.%4</h4>\n")
                    .arg(i18n("Runtime version"))
                    .arg(QTAV_VERSION_MAJOR(rt_version))
                    .arg(QTAV_VERSION_MINOR(rt_version))
                    .arg(QTAV_VERSION_PATCH(rt_version))
            ;
        }

        text += QString::fromUtf8("<p>%1</p>\n<p>%2</p>\n")
            .arg(QString::fromUtf8(info->config))
            .arg(QString::fromUtf8(info->license));

        info  = Internal::get_depend_component(info);
    }

    return text;
}

QString aboutQtAV_PlainText()
{

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

    return aboutQtAV_HTML().remove(QRegularExpression(QString::fromUtf8("<[^>]*>")));

#else

    return aboutQtAV_HTML().remove(QRegExp(QString::fromUtf8("<[^>]*>")));

#endif

}

QString aboutQtAV_HTML()
{
    static QString about = QString::fromLatin1("<h3>QtAV " QTAV_VERSION_STR_LONG "</h3>\n<p>%1</p>")
                           .arg(i18n("Multimedia framework based on Qt and FFmpeg.\n"));

    return about;
}

void setFFmpegLogHandler(void (*callback)(void *, int, const char *, va_list))
{
    // libav does not check null callback

    if (!callback)
        callback = av_log_default_callback;

    av_log_set_callback(callback);
}

void setFFmpegLogLevel(const QByteArray& level)
{
    if (level.isEmpty())
        return;

    bool ok         = false;
    const int value = level.toInt(&ok);

    if      ((ok && value == 0) || (level == "off") || (level == "quiet"))
        Internal::gAVLogLevel = AV_LOG_QUIET;
    else if (level == "panic")
        Internal::gAVLogLevel = AV_LOG_PANIC;
    else if (level == "fatal")
        Internal::gAVLogLevel = AV_LOG_FATAL;
    else if (level == "error")
        Internal::gAVLogLevel = AV_LOG_ERROR;
    else if (level.startsWith(QByteArray("warn")))
        Internal::gAVLogLevel = AV_LOG_WARNING;
    else if (level == "info")
        Internal::gAVLogLevel = AV_LOG_INFO;
    else if (level == "verbose")
        Internal::gAVLogLevel = AV_LOG_VERBOSE;
    else if (level == "debug")
        Internal::gAVLogLevel = AV_LOG_DEBUG;

#ifdef AV_LOG_TRACE

    else if (level == "trace")
        Internal::gAVLogLevel = AV_LOG_TRACE;

#endif

    else
        Internal::gAVLogLevel = AV_LOG_INFO;

    av_log_set_level(Internal::gAVLogLevel);
}

static void qtav_ffmpeg_log_callback(void* ctx, int level,const char* fmt, va_list vl)
{
    // AV_LOG_DEBUG is used by ffmpeg developers

    if (level > Internal::gAVLogLevel)
        return;

    AVClass* const c = ctx ? *(AVClass**)ctx : nullptr;
    QString qmsg     = QString().asprintf("[FFmpeg:%s] ", c ? c->item_name(ctx) : "?") + QString().vasprintf(fmt, vl);
    qmsg             = qmsg.trimmed();

    if      (level > AV_LOG_WARNING)
        qCDebug(DIGIKAM_QTAV_LOG) << qPrintable(qmsg);
    else if (level > AV_LOG_PANIC)
        qCWarning(DIGIKAM_QTAV_LOG_WARN) << qPrintable(qmsg);
}

QString avformatOptions()
{
    static QString opts;

    if (!opts.isEmpty())
        return opts;

    void* const obj = const_cast<void*>(reinterpret_cast<const void*>(avformat_get_class()));
    opts            = Internal::optionsToString((void*)&obj);
    opts.append(ushort('\n'));

#if AVFORMAT_STATIC_REGISTER

    const AVInputFormat* i = nullptr;
    void* it               = nullptr;

    while ((i = av_demuxer_iterate(&it)))
    {

#else

    AVInputFormat* i       = nullptr;
    av_register_all();                 // MUST register all input/output formats

    while ((i = av_iformat_next(i)))
    {

#endif

        QString opt(Internal::optionsToString((void*)&i->priv_class).trimmed());

        if (opt.isEmpty())
            continue;

        opts.append(QString::fromUtf8("options for input format %1:\n%2\n\n")
                    .arg(QLatin1String(i->name))
                    .arg(opt));
    }

#if AVFORMAT_STATIC_REGISTER

    const AVOutputFormat* o = nullptr;
    it                      = nullptr;

    while ((o = av_muxer_iterate(&it)))
    {

#else

    av_register_all();                  // MUST register all input/output formats
    AVOutputFormat* o       = nullptr;

    while ((o = av_oformat_next(o)))
    {

#endif

        QString opt(Internal::optionsToString((void*)&o->priv_class).trimmed());

        if (opt.isEmpty())
            continue;

        opts.append(QString::fromUtf8("options for output format %1:\n%2\n\n")
                    .arg(QLatin1String(o->name))
                    .arg(opt));
    }

    return opts;
}

QString avcodecOptions()
{
    static QString opts;

    if (!opts.isEmpty())
        return opts;

    void* const obj  = const_cast<void*>(reinterpret_cast<const void*>(avcodec_get_class()));
    opts             = Internal::optionsToString((void*)&obj);
    opts.append(ushort('\n'));
    const AVCodec* c = nullptr;

#if AVCODEC_STATIC_REGISTER

    void* it = nullptr;

    while ((c = av_codec_iterate(&it)))
    {

#else

    avcodec_register_all();

    while ((c = av_codec_next(c)))
    {

#endif

        QString opt(Internal::optionsToString((void*)&c->priv_class).trimmed());

        if (opt.isEmpty())
            continue;

        opts.append(QString::fromUtf8("Options for codec %1:\n%2\n\n")
            .arg(QLatin1String(c->name))
            .arg(opt));
    }

    return opts;
}

#if 0

const QStringList& supportedInputMimeTypes()
{
    static QStringList mimes;

    if (!mimes.isEmpty())
        return mimes;

    av_register_all();                          // MUST register all input/output formats
    AVOutputFormat* i = av_oformat_next(nullptr);
    QStringList list;

    while (i)
    {
        list << QString(i->mime_type).split(QLatin1Char(','), QString::SkipEmptyParts);
        i = av_oformat_next(i);
    }

    Q_FOREACH (const QString& v, list)
    {
        mimes.append(v.trimmed());
    }

    mimes.removeDuplicates();

    return mimes;
}

static QStringList s_audio_mimes, s_video_mimes, s_subtitle_mimes;

static void init_supported_codec_info()
{
    const AVCodecDescriptor* cd = avcodec_descriptor_next(nullptr);

    while (cd)
    {
        QStringList list;

        if (cd->mime_types)
        {
            for (int i = 0 ; cd->mime_types[i] ; ++i)
            {
                list.append(QString(cd->mime_types[i]).trimmed());
            }
        }

        switch (cd->type)
        {
            case AVMEDIA_TYPE_AUDIO:
            {
                s_audio_mimes << list;

                break;
            }

            case AVMEDIA_TYPE_VIDEO:
            {
                s_video_mimes << list;

                break;
            }

            case AVMEDIA_TYPE_SUBTITLE:
            {
                s_subtitle_mimes << list;

                break;
            }

            default:
            {
                break;
            }
        }

        cd = avcodec_descriptor_next(cd);
    }

    s_audio_mimes.removeDuplicates();
    s_video_mimes.removeDuplicates();
    s_subtitle_mimes.removeDuplicates();
}

const QStringList& supportedAudioMimeTypes()
{
    if (s_audio_mimes.isEmpty())
        init_supported_codec_info();

    return s_audio_mimes;
}

const QStringList& supportedVideoMimeTypes()
{
    if (s_video_mimes.isEmpty())
        init_supported_codec_info();

    return s_video_mimes;
}

// TODO: subtitleprocessor support

const QStringList& supportedSubtitleMimeTypes()
{
    if (s_subtitle_mimes.isEmpty())
        init_supported_codec_info();

    return s_subtitle_mimes;
}

#endif // 0

/*
 * AVColorSpace:
 * libav11 libavutil54.3.0 pixfmt.h, ffmpeg2.1*libavutil52.48.101 frame.h
 * ffmpeg2.5 pixfmt.h. AVFrame.colorspace
 * earlier versions: avcodec.h, avctx.colorspace
 */
ColorSpace colorSpaceFromFFmpeg(AVColorSpace cs)
{
    switch (cs)
    {
        // from ffmpeg: order of coefficients is actually GBR

        case AVCOL_SPC_RGB:
        {
            return ColorSpace_GBR;
        }

        case AVCOL_SPC_BT709:
        {
            return ColorSpace_BT709;
        }

        case AVCOL_SPC_BT470BG:
        {
            return ColorSpace_BT601;
        }

        case AVCOL_SPC_SMPTE170M:
        {
            return ColorSpace_BT601;
        }

        default:
        {
            return ColorSpace_Unknown;
        }
    }
}

ColorRange colorRangeFromFFmpeg(AVColorRange cr)
{
    switch (cr)
    {
        case AVCOL_RANGE_MPEG:
        {
            return ColorRange_Limited;
        }

        case AVCOL_RANGE_JPEG:
        {
            return ColorRange_Full;
        }

        default:
        {
            return ColorRange_Unknown;
        }
    }
}

namespace
{

static const struct Q_DECL_HIDDEN RegisterMetaTypes
{
    RegisterMetaTypes()
    {
        qRegisterMetaType<QtAV::MediaStatus>("QtAV::MediaStatus");
    }
} _registerMetaTypes;

}

// TODO: static link. move all into 1

namespace
{

class Q_DECL_HIDDEN InitFFmpegLog
{
public:

    InitFFmpegLog()
    {
        setFFmpegLogHandler(qtav_ffmpeg_log_callback);
        QtAV::setFFmpegLogLevel(qgetenv("QTAV_FFMPEG_LOG").toLower());
    }
};

InitFFmpegLog fflog;

} // namespace

} // namespace QtAV

// Initialize Qt Resource System when the library is built statically

static void initResources()
{
    Q_INIT_RESOURCE(shaders);
}

namespace
{
    class ResourceLoader
    {
    public:

        ResourceLoader()
        {
            initResources();
        }
    };

    ResourceLoader QtAV_QRCLoader;

} // namespace
