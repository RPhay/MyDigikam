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

#include "LibAVFilter.h"

// Qt includes

#include <QSharedPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "Filter_p.h"
#include "QtAV_Statistics.h"
#include "AudioFrame.h"
#include "VideoFrame.h"
#include "AVCompat.h"
#include "QtAV_internal.h"
#include "digikam_debug.h"

// Turn to 1 to debug libav filter graph.

#define DBG_GRAPH 0

/*
 * libav10.x, ffmpeg2.x: av_buffersink_read deprecated
 * libav9.x: only av_buffersink_read can be used
 * ffmpeg<2.0: av_buffersink_get_buffer_ref and av_buffersink_read
 */

// TODO: enabled = false if no libavfilter

// TODO: filter_complex

// NO COPY in push/pull

#define QTAV_HAVE_av_buffersink_get_frame (LIBAV_MODULE_CHECK(LIBAVFILTER, 4, 2, 0) || FFMPEG_MODULE_CHECK(LIBAVFILTER, 3, 79, 100)) // 3.79.101: ff2.0.4

namespace QtAV
{

#if QTAV_HAVE(AVFILTER)

// local types can not be used as template parameters

class Q_DECL_HIDDEN AVFrameHolder
{
public:

    AVFrameHolder()
    {
        m_frame = av_frame_alloc();

#if !QTAV_HAVE_av_buffersink_get_frame

        picref  = 0;

#endif

    }

    ~AVFrameHolder()
    {
        av_frame_free(&m_frame);

#if !QTAV_HAVE_av_buffersink_get_frame

        avfilter_unref_bufferp(&picref);

#endif

    }

    AVFrame* frame()
    {
        return m_frame;
    }

#if !QTAV_HAVE_av_buffersink_get_frame

    AVFilterBufferRef** bufferRef()
    {
        return &picref;
    }

    // copy properties and data ptrs(no deep copy).

    void copyBufferToFrame()
    {
        avfilter_copy_buf_props(m_frame, picref);
    }

#endif

private:

    AVFrame*            m_frame = nullptr;

#if !QTAV_HAVE_av_buffersink_get_frame

    AVFilterBufferRef*  picref  = nullptr;

#endif

};

typedef QSharedPointer<AVFrameHolder> AVFrameHolderRef;

#endif // QTAV_HAVE(AVFILTER)


class Q_DECL_HIDDEN LibAVFilter::Private
{
public:

    Private()
        : avframe(nullptr),
          status (LibAVFilter::NotConfigured)
    {

#if QTAV_HAVE(AVFILTER)

        filter_graph   = nullptr;
        in_filter_ctx  = nullptr;
        out_filter_ctx = nullptr;

#   if LIBAVCODEC_VERSION_MAJOR < 59

        avfilter_register_all();

#   endif

#endif // QTAV_HAVE(AVFILTER)

    }

    ~Private()
    {

#if QTAV_HAVE(AVFILTER)

        avfilter_graph_free(&filter_graph);

#endif // QTAV_HAVE(AVFILTER)

        if (avframe)
        {
            av_frame_free(&avframe);
            avframe = nullptr;
        }
    }

    bool setOptions(const QString& opt)
    {
        if (options == opt)
            return false;

        options = opt;
        status  = LibAVFilter::NotConfigured;

        return true;
    }

    bool pushAudioFrame(Frame* frame, bool changed, const QString& args);
    bool pushVideoFrame(Frame* frame, bool changed, const QString& args);

    bool setup(const QString& args, bool video)
    {
        if (avframe)
        {
            av_frame_free(&avframe);
            avframe = nullptr;
        }

        status       = LibAVFilter::ConfigureFailed;

#if QTAV_HAVE(AVFILTER)

        avfilter_graph_free(&filter_graph);
        filter_graph = avfilter_graph_alloc();

        //QString sws_flags_str;

        // pixel_aspect==sar, pixel_aspect is more compatible

        QString buffersrc_args = args;

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("buffersrc_args=%s",
                buffersrc_args.toUtf8().constData());

#if LIBAVFILTER_VERSION_INT >= AV_VERSION_INT(7,0,0)

        const

#endif

        AVFilter* const buffersrc = avfilter_get_by_name(video ? "buffer" : "abuffer");

        Q_ASSERT(buffersrc);
        AV_ENSURE_OK(avfilter_graph_create_filter(&in_filter_ctx,
                                                  buffersrc,
                                                  "in", buffersrc_args.toUtf8().constData(), nullptr,
                                                  filter_graph)
                     , false);

        // buffer video sink: to terminate the filter chain.

#if LIBAVFILTER_VERSION_INT >= AV_VERSION_INT(7,0,0)

        const

#endif

        AVFilter* const buffersink = avfilter_get_by_name(video ? "buffersink" : "abuffersink");

        Q_ASSERT(buffersink);
        AV_ENSURE_OK(avfilter_graph_create_filter(&out_filter_ctx, buffersink, "out",
                                                  nullptr, nullptr, filter_graph),
                     false);

        // Endpoints for the filter graph.

        AVFilterInOut* outputs = avfilter_inout_alloc();
        AVFilterInOut* inputs  = avfilter_inout_alloc();
        outputs->name          = av_strdup("in");
        outputs->filter_ctx    = in_filter_ctx;
        outputs->pad_idx       = 0;
        outputs->next          = nullptr;

        inputs->name           = av_strdup("out");
        inputs->filter_ctx     = out_filter_ctx;
        inputs->pad_idx        = 0;
        inputs->next           = nullptr;

        struct Q_DECL_HIDDEN delete_helper
        {
            AVFilterInOut** x;

            delete_helper(AVFilterInOut** io)
                : x(io)
            {
            }

            ~delete_helper()
            {
                // libav always free it in avfilter_graph_parse. so we does nothing

#if QTAV_USE_FFMPEG(LIBAVFILTER)

                avfilter_inout_free(x);

#endif

            }
        } scoped_in(&inputs), scoped_out(&outputs);

        // avfilter_graph_parse, avfilter_graph_parse2?

        AV_ENSURE_OK(avfilter_graph_parse_ptr(filter_graph, options.toUtf8().constData(),
                                              &inputs, &outputs, nullptr), false);
        AV_ENSURE_OK(avfilter_graph_config(filter_graph, nullptr), false);
        avframe = av_frame_alloc();
        status  = LibAVFilter::ConfigureOk;

#if DBG_GRAPH

        // not available in libav9

        const char* g = avfilter_graph_dump(filter_graph, nullptr);

        if (g)
        {
            qCDebug(DIGIKAM_QTAV_LOG).nospace()
                << "filter graph:\n" << g; // use << to not print special chars in qt5.5
        }

        av_freep(&g);

#endif // DBG_GRAPH

        return true;

#endif // QTAV_HAVE(AVFILTER)

        return false;
    }

#if QTAV_HAVE(AVFILTER)

    AVFilterGraph*      filter_graph   = nullptr;
    AVFilterContext*    in_filter_ctx  = nullptr;
    AVFilterContext*    out_filter_ctx = nullptr;

#endif // QTAV_HAVE(AVFILTER)

    AVFrame*            avframe        = nullptr;
    QString             options;
    LibAVFilter::Status status;
};

QStringList LibAVFilter::videoFilters()
{
    static const QStringList list(LibAVFilter::registeredFilters(AVMEDIA_TYPE_VIDEO));

    return list;
}

QStringList LibAVFilter::audioFilters()
{
    static const QStringList list(LibAVFilter::registeredFilters(AVMEDIA_TYPE_AUDIO));

    return list;
}

QString LibAVFilter::filterDescription(const QString& filterName)
{
    QString s;

#if QTAV_HAVE(AVFILTER)

#   if LIBAVCODEC_VERSION_MAJOR < 59

    avfilter_register_all();

#   endif

    const AVFilter* f = avfilter_get_by_name(filterName.toUtf8().constData());

    if (!f)
    {
        return s;
    }

    if (f->description)
    {
        s.append(QString::fromUtf8(f->description));
    }

#if AV_MODULE_CHECK(LIBAVFILTER, 3, 7, 0, 8, 100)

    return (s.append(QLatin1String("\n")).append(i18n("Options:"))
             .append(Internal::optionsToString((void*)&f->priv_class)));

#endif

#endif // QTAV_HAVE(AVFILTER)

    Q_UNUSED(filterName);

    return s;
}

LibAVFilter::LibAVFilter()
    : priv(new Private())
{
}

LibAVFilter::~LibAVFilter()
{
    delete priv;
}

void LibAVFilter::setOptions(const QString& options)
{
    if (!priv->setOptions(options))
        return;

    emitOptionsChanged();
}

QString LibAVFilter::options() const
{
    return priv->options;
}

LibAVFilter::Status LibAVFilter::status() const
{
    return priv->status;
}

bool LibAVFilter::pushVideoFrame(Frame* frame, bool changed)
{
    return priv->pushVideoFrame(frame, changed, sourceArguments());
}

bool LibAVFilter::pushAudioFrame(Frame* frame, bool changed)
{
    return priv->pushAudioFrame(frame, changed, sourceArguments());
}

void* LibAVFilter::pullFrameHolder()
{

#if QTAV_HAVE(AVFILTER)

    AVFrameHolder* holder = nullptr;
    holder                = new AVFrameHolder();

#if QTAV_HAVE_av_buffersink_get_frame

    int ret = av_buffersink_get_frame(priv->out_filter_ctx, holder->frame());

#else

    int ret = av_buffersink_read(priv->out_filter_ctx, holder->bufferRef());

#endif // QTAV_HAVE_av_buffersink_get_frame

    if (ret < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("av_buffersink_get_frame error: %s",
                av_err2str(ret));

        delete holder;

        return nullptr;
    }

#if !QTAV_HAVE_av_buffersink_get_frame

    holder->copyBufferToFrame();

#endif

    return holder;

#endif // QTAV_HAVE(AVFILTER)

    return nullptr;
}

QStringList LibAVFilter::registeredFilters(int type)
{
    QStringList filters = QStringList();

#if QTAV_HAVE(AVFILTER)

#   if LIBAVCODEC_VERSION_MAJOR < 59

    avfilter_register_all();

#   endif

    const AVFilter* f = nullptr;
    AVFilterPad* fp   = nullptr; // no const in avfilter_pad_get_name() for ffmpeg<=1.2 libav<=9

#   if AV_MODULE_CHECK(LIBAVFILTER, 3, 8, 0, 53, 100)

#       if LIBAVCODEC_VERSION_MAJOR < 59

    while ((f = avfilter_next(f)))
    {

#       else // ffmpeg >= 5

    void* ff          = nullptr;

    while ((f = av_filter_iterate(&ff)))
    {

#       endif

#   else

    AVFilter** ff     = nullptr;

    while ((ff = av_filter_next(ff)) && *ff)
    {
        f = (*ff);

#   endif

        fp = (AVFilterPad*)f->inputs;

        // only check the 1st pad

        if (!fp || !avfilter_pad_get_name(fp, 0) || (avfilter_pad_get_type(fp, 0) != (AVMediaType)type))
            continue;

        fp = (AVFilterPad*)f->outputs;

        // only check the 1st pad

        if (!fp || !avfilter_pad_get_name(fp, 0) || (avfilter_pad_get_type(fp, 0) != (AVMediaType)type))
            continue;

        filters.append(QLatin1String(f->name));
    }

#endif // QTAV_HAVE(AVFILTER)

    return filters;
}

// ------------------------------------------------------------------------------

class Q_DECL_HIDDEN LibAVFilterVideoPrivate : public VideoFilterPrivate
{
public:

    LibAVFilterVideoPrivate()
        : VideoFilterPrivate(),
          pixfmt            (QTAV_PIX_FMT_C(NONE)),
          width             (0),
          height            (0)
    {
    }

    AVPixelFormat pixfmt;
    int           width;
    int           height;

private:

    Q_DISABLE_COPY(LibAVFilterVideoPrivate);
};

LibAVFilterVideo::LibAVFilterVideo(QObject* const parent)
    : VideoFilter(*new LibAVFilterVideoPrivate(), parent),
      LibAVFilter()
{
}

QStringList LibAVFilterVideo::filters() const
{
    return LibAVFilter::videoFilters();
}

void LibAVFilterVideo::process(Statistics* statistics, VideoFrame* frame)
{
    Q_UNUSED(statistics);

#if QTAV_HAVE(AVFILTER)

    if (status() == ConfigureFailed)
        return;

    DPTR_D(LibAVFilterVideo);
/*
    Status old = status();
*/
    bool changed = false;

    if ((d.width != frame->width()) || (d.height != frame->height()) || (d.pixfmt != frame->pixelFormatFFmpeg()))
    {
        changed  = true;
        d.width  = frame->width();
        d.height = frame->height();
        d.pixfmt = (AVPixelFormat)frame->pixelFormatFFmpeg();
    }

    bool ok = pushVideoFrame(frame, changed);
/*
    if (old != status())
        Q_EMIT statusChanged();
*/
    if (!ok)
        return;

    AVFrameHolderRef ref((AVFrameHolder*)pullFrameHolder());

    if (!ref)
        return;

    const AVFrame* f = ref->frame();
    VideoFrame vf(f->width, f->height, VideoFormat(f->format));
    vf.setBits((quint8**)f->data);
    vf.setBytesPerLine((int*)f->linesize);
    vf.setMetaData(QString::fromUtf8("avframe_hoder_ref"), QVariant::fromValue(ref));
    vf.setTimestamp(ref->frame()->pts / 1000000.0); // pkt_pts?
/*
    vf.setMetaData(frame->availableMetaData());
*/
    *frame = vf;

#else

    Q_UNUSED(frame);

#endif // QTAV_HAVE(AVFILTER)

}

QString LibAVFilterVideo::sourceArguments() const
{
    DPTR_D(const LibAVFilterVideo);

#if QTAV_USE_LIBAV(LIBAVFILTER)

    return QString::fromUtf8("%1:%2:%3:%4:%5:%6:%7")

#else

    return QString::fromUtf8("video_size=%1x%2:pix_fmt=%3:time_base=%4/%5:pixel_aspect=%6/%7")

#endif

            .arg(d.width).arg(d.height).arg(d.pixfmt)
            .arg(1).arg(AV_TIME_BASE)   // time base 1/1?
            .arg(1).arg(1)              // sar
    ;
}

void LibAVFilterVideo::emitOptionsChanged()
{
    Q_EMIT optionsChanged();
}

// ------------------------------------------------------------------------------

class Q_DECL_HIDDEN LibAVFilterAudioPrivate : public AudioFilterPrivate
{
public:

    LibAVFilterAudioPrivate()
        : AudioFilterPrivate(),
          sample_rate       (0),
          sample_fmt        (AV_SAMPLE_FMT_NONE),
          channel_layout    (0)
    {
    }

    int            sample_rate;
    AVSampleFormat sample_fmt;
    qint64         channel_layout;

private:

    Q_DISABLE_COPY(LibAVFilterAudioPrivate);
};

LibAVFilterAudio::LibAVFilterAudio(QObject* const parent)
    : AudioFilter(*new LibAVFilterAudioPrivate(), parent),
      LibAVFilter()
{
}

QStringList LibAVFilterAudio::filters() const
{
    return LibAVFilter::audioFilters();
}

QString LibAVFilterAudio::sourceArguments() const
{
    DPTR_D(const LibAVFilterAudio);

    return QString::fromUtf8("time_base=%1/%2:sample_rate=%3:sample_fmt=%4:channel_layout=0x%5")
            .arg(1)
            .arg(AV_TIME_BASE)
            .arg(d.sample_rate)

            // ffmpeg new: AV_OPT_TYPE_SAMPLE_FMT
            // libav, ffmpeg old: AV_OPT_TYPE_STRING

            .arg(QLatin1String(av_get_sample_fmt_name(d.sample_fmt)))
            .arg(d.channel_layout, 0, 16) // AV_OPT_TYPE_STRING
    ;
}

void LibAVFilterAudio::process(Statistics* statistics, AudioFrame* frame)
{
    Q_UNUSED(statistics);

#if QTAV_HAVE(AVFILTER)

    if (status() == ConfigureFailed)
        return;

    DPTR_D(LibAVFilterAudio);
/*
    Status old   = status();
*/
    bool changed = false;
    const AudioFormat afmt(frame->format());

    if ((d.sample_rate    != afmt.sampleRate())         ||
        (d.sample_fmt     != afmt.sampleFormatFFmpeg()) ||
        (d.channel_layout != afmt.channelLayoutFFmpeg()))
    {
        changed          = true;
        d.sample_rate    = afmt.sampleRate();
        d.sample_fmt     = (AVSampleFormat)afmt.sampleFormatFFmpeg();
        d.channel_layout = afmt.channelLayoutFFmpeg();
    }

    bool ok      = pushAudioFrame(frame, changed);
/*
    if (old != status())
        Q_EMIT statusChanged();
*/
    if (!ok)
        return;

    AVFrameHolderRef ref((AVFrameHolder*)pullFrameHolder());

    if (!ref)
        return;

    const AVFrame* f = ref->frame();
    AudioFormat fmt;
    fmt.setSampleFormatFFmpeg(f->format);
    fmt.setChannelLayoutFFmpeg(f->channel_layout);
    fmt.setSampleRate(f->sample_rate);

    if (!fmt.isValid())
    {
        // need more data to decode to get a frame

        return;
    }

    AudioFrame af(fmt);
/*
    af.setBits((quint8**)f->extended_data);
    af.setBytesPerLine((int*)f->linesize);
*/
    af.setBits(f->extended_data);                     // TODO: ref
    af.setBytesPerLine(f->linesize[0], 0);            // for correct alignment
    af.setSamplesPerChannel(f->nb_samples);
    af.setMetaData(QLatin1String("avframe_hoder_ref"), QVariant::fromValue(ref));
    af.setTimestamp(ref->frame()->pts / 1000000.0);   // pkt_pts?
/*
    af.setMetaData(frame->availableMetaData());
*/
    *frame = af;

#else

    Q_UNUSED(frame);

#endif // QTAV_HAVE(AVFILTER)

}

void LibAVFilterAudio::emitOptionsChanged()
{
    Q_EMIT optionsChanged();
}

// ------------------------------------------------------------------------------

bool LibAVFilter::Private::pushVideoFrame(Frame* frame, bool changed, const QString& args)
{

#if QTAV_HAVE(AVFILTER)

    VideoFrame* const vf = static_cast<VideoFrame*>(frame);

    if ((status == LibAVFilter::NotConfigured) || !avframe || changed)
    {
        if (!setup(args, true))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("setup video filter graph error");
/*
            enabled = false; // skip this filter and avoid crash
*/
            return false;
        }
    }

    if (!vf->constBits(0))
    {
        *vf = vf->to(vf->format());
    }

    avframe->pts    = frame->timestamp() * 1000000.0; // time_base is 1/1000000
    avframe->width  = vf->width();
    avframe->height = vf->height();
    avframe->format = (AVPixelFormat)vf->pixelFormatFFmpeg();

    for (int i = 0 ; i < vf->planeCount() ; ++i)
    {
        avframe->data[i]     = (uint8_t*)vf->constBits(i);
        avframe->linesize[i] = vf->bytesPerLine(i);
    }

    // TODO: side data for vf_codecview etc
/*
    int ret = av_buffersrc_add_frame_flags(in_filter_ctx, avframe, AV_BUFFERSRC_FLAG_KEEP_REF);
*/
    /*
     * av_buffersrc_write_frame equals to av_buffersrc_add_frame_flags with AV_BUFFERSRC_FLAG_KEEP_REF.
     * av_buffersrc_write_frame is more compatible, while av_buffersrc_add_frame_flags only exists in ffmpeg >=2.0
     * add a ref if frame is ref counted
     * TODO: libav < 10.0 will copy the frame, prefer to use av_buffersrc_buffer
     */

    AV_ENSURE_OK(av_buffersrc_write_frame(in_filter_ctx, avframe), false);

    return true;

#endif // QTAV_HAVE(AVFILTER)

    Q_UNUSED(frame);

    return false;
}

bool LibAVFilter::Private::pushAudioFrame(Frame* frame, bool changed, const QString& args)
{

#if QTAV_HAVE(AVFILTER)

    if ((status == LibAVFilter::NotConfigured) || !avframe || changed)
    {
        if (!setup(args, false))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("setup audio filter graph error");
/*
            enabled = false; // skip this filter and avoid crash
*/
            return false;
        }
    }

    AudioFrame* const af    = static_cast<AudioFrame*>(frame);
    const AudioFormat afmt(af->format());
    avframe->pts            = frame->timestamp() * 1000000.0; // time_base is 1/1000000
    avframe->sample_rate    = afmt.sampleRate();
    avframe->channel_layout = afmt.channelLayoutFFmpeg();

#if QTAV_USE_FFMPEG(LIBAVCODEC) || QTAV_USE_FFMPEG(LIBAVUTIL) // AVFrame was in avcodec

    avframe->channels       = afmt.channels(); // MUST set because av_buffersrc_write_frame will compare channels and layout

#endif

    avframe->format         = (AVSampleFormat)afmt.sampleFormatFFmpeg();
    avframe->nb_samples     = af->samplesPerChannel();

    for (int i = 0 ; i < af->planeCount() ; ++i)
    {
/*
        avframe->data[i] = (uint8_t*)af->constBits(i);
*/
        avframe->extended_data[i] = (uint8_t*)af->constBits(i);
        avframe->linesize[i]      = af->bytesPerLine(i);
    }

    AV_ENSURE_OK(av_buffersrc_write_frame(in_filter_ctx, avframe), false);

    return true;

#endif // QTAV_HAVE(AVFILTER)

    Q_UNUSED(frame);

    return false;
}

} // namespace QtAV

#if QTAV_HAVE(AVFILTER)

Q_DECLARE_METATYPE(QtAV::AVFrameHolderRef)

#endif
