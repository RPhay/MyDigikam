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

#include "SubtitleProcessor.h"

// Qt includes

#include <QIODevice>

// Local includes

#include "QtAV_factory.h"
#include "AVDemuxer.h"
#include "Packet.h"
#include "AVCompat.h"
#include "PlainText.h"
#include "digikam_debug.h"

namespace QtAV
{

class Q_DECL_HIDDEN SubtitleProcessorFFmpeg final : public SubtitleProcessor
{
public:

    SubtitleProcessorFFmpeg();
    ~SubtitleProcessorFFmpeg();

    SubtitleProcessorId id()                const override;
    QString name()                          const override;
    QStringList supportedTypes()            const override;
    bool process(QIODevice* dev)                  override;

    // supportsFromFile must be true

    bool process(const QString& path)             override;
    QList<SubtitleFrame> frames()           const override;
    bool processHeader(const QByteArray& codec,
                       const QByteArray& data)    override;

    SubtitleFrame processLine(const QByteArray& data,
                              qreal pts = -1,
                              qreal duration = 0) override;

    QString getText(qreal pts)              const override;

private:

    bool processSubtitle();

private:

    AVCodecContext*      codec_ctx = nullptr;
    AVDemuxer            m_reader;
    QList<SubtitleFrame> m_frames;
};

static const SubtitleProcessorId SubtitleProcessorId_FFmpeg = QLatin1String("qtav.subtitle.processor.ffmpeg");

namespace
{

static const char kName[] = "FFmpeg";

} // namespace

FACTORY_REGISTER(SubtitleProcessor, FFmpeg, kName)

SubtitleProcessorFFmpeg::SubtitleProcessorFFmpeg()
    : codec_ctx(nullptr)
{
}

SubtitleProcessorFFmpeg::~SubtitleProcessorFFmpeg()
{
    avcodec_free_context(&codec_ctx);
}

SubtitleProcessorId SubtitleProcessorFFmpeg::id() const
{
    return SubtitleProcessorId_FFmpeg;
}

QString SubtitleProcessorFFmpeg::name() const
{
    return QLatin1String(kName); // SubtitleProcessorFactory::name(id());
}

QStringList ffmpeg_supported_sub_extensions_by_codec()
{
    QStringList exts;
    const AVCodec* c = nullptr;

#if AVCODEC_STATIC_REGISTER

    void* it         = nullptr;

    while ((c = av_codec_iterate(&it)))
    {

#else

    avcodec_register_all();

    while ((c = av_codec_next(c)))
    {

#endif
        if (c->type != AVMEDIA_TYPE_SUBTITLE)
            continue;

        qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("sub codec: %s", c->name);

#if AVFORMAT_STATIC_REGISTER

        const AVInputFormat* i = nullptr;
        void* it2              = nullptr;

        while ((i = av_demuxer_iterate(&it2)))
        {

#else

        av_register_all();              // MUST register all input/output formats
        AVInputFormat* i = nullptr;

        while ((i = av_iformat_next(i)))
        {

#endif
            if (!strcmp(i->name, c->name))
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("found iformat");

                if (i->extensions)
                {
                    exts.append(QString::fromLatin1(i->extensions).split(QLatin1Char(',')));
                }
                else
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("has no exts");

                    exts.append(QString::fromLatin1(i->name));
                }

                break;
            }
        }

        if (!i)
        {
/*
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("codec name '%s' is not found in AVInputFormat,
                    just append codec name", c->name);

            exts.append(c->name);
*/
        }
    }

    return exts;
}

QStringList ffmpeg_supported_sub_extensions()
{
    QStringList exts;

#if AVFORMAT_STATIC_REGISTER

    const AVInputFormat* i = nullptr;
    void* it               = nullptr;

    while ((i = av_demuxer_iterate(&it)))
    {

#else

    av_register_all(); // MUST register all input/output formats
    AVInputFormat* i = nullptr;

    while ((i = av_iformat_next(i)))
    {

#endif
        // strstr parameters can not be null

        if (i->long_name && strstr(i->long_name, "subtitle"))
        {
            if (i->extensions)
            {
                exts.append(QString::fromLatin1(i->extensions).split(QLatin1Char(',')));
            }
            else
            {
                exts.append(QString::fromLatin1(i->name));
            }
        }
    }

    // AVCodecDescriptor.name and AVCodec.name may be different. avcodec_get_name() use AVCodecDescriptor if possible

    QStringList codecs;
    const AVCodec* c = nullptr;

#if AVCODEC_STATIC_REGISTER

    it = nullptr;

    while ((c = av_codec_iterate(&it)))
    {

#else

    avcodec_register_all();

    while ((c = av_codec_next(c)))
    {

#endif

        if (c->type == AVMEDIA_TYPE_SUBTITLE)
            codecs.append(QString::fromLatin1(c->name));
    }

    const AVCodecDescriptor* desc = nullptr;

    while ((desc = avcodec_descriptor_next(desc)))
    {
        if (desc->type == AVMEDIA_TYPE_SUBTITLE)
            codecs.append(QString::fromLatin1(desc->name));
    }

    exts << codecs;
    exts.removeDuplicates();

    return exts;
}

QStringList SubtitleProcessorFFmpeg::supportedTypes() const
{
    // TODO: mp4. check avformat classes

#if 0

    typedef struct Q_DECL_HIDDEN
    {
        const char* ext  = nullptr;
        const char* name = nullptr;
    } sub_ext_t;

    static const sub_ext_t sub_ext[] =
    {
        { "ass", "ass"       },
        { "ssa", "ass"       },
        { "sub", "subviewer" },
        { ""                 }
    };

    // from ffmpeg/tests/fate/subtitles.mak

    static const QStringList sSuffixes = QStringList() << "ass" << "ssa" << "sub" << "srt"
                                                       << "txt" << "vtt" << "smi" << "pjs"
                                                       << "jss" << "aqt";

#endif

    static const QStringList sSuffixes = ffmpeg_supported_sub_extensions();

    return sSuffixes;
}

bool SubtitleProcessorFFmpeg::process(QIODevice* dev)
{
    if (!dev->isOpen())
    {
        if (!dev->open(QIODevice::ReadOnly))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN) << "open qiodevice error: "
                                             << dev->errorString();

            return false;
        }
    }

    m_reader.setMedia(dev);

    if (!m_reader.load())
        goto error;

    if (m_reader.subtitleStreams().isEmpty())
        goto error;

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("subtitle format: %s",
            m_reader.formatContext()->iformat->name);

    if (!processSubtitle())
        goto error;

    m_reader.unload();

    return true;

error:

    m_reader.unload();

    return false;
}

bool SubtitleProcessorFFmpeg::process(const QString& path)
{
    m_reader.setMedia(path);

    if (!m_reader.load())
        goto error;

    if (m_reader.subtitleStreams().isEmpty())
        goto error;

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("subtitle format: %s",
            m_reader.formatContext()->iformat->name);

    if (!processSubtitle())
        goto error;

    m_reader.unload();

    return true;

error:

    m_reader.unload();

    return false;
}

QList<SubtitleFrame> SubtitleProcessorFFmpeg::frames() const
{
    return m_frames;
}

QString SubtitleProcessorFFmpeg::getText(qreal pts) const
{
    QString text;

    for (int i = 0 ; i < m_frames.size() ; ++i)
    {
        if ((m_frames[i].begin <= pts) && (m_frames[i].end >= pts))
        {
            text += m_frames[i].text + QLatin1Char('\n');

            continue;
        }

        if (!text.isEmpty())
            break;
    }

    return text.trimmed();
}

bool SubtitleProcessorFFmpeg::processHeader(const QByteArray& codec, const QByteArray& data)
{
    Q_UNUSED(data);

    if (codec_ctx)
    {
        avcodec_free_context(&codec_ctx);
    }

#ifndef HAVE_FFMPEG_VERSION5

    AVCodec* c       = avcodec_find_decoder_by_name(codec.constData());

#else // ffmpeg >= 5

    const AVCodec* c = avcodec_find_decoder_by_name(codec.constData());

#endif

    if (!c)
    {
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("subtitle avcodec_descriptor_get_by_name %s",
                codec.constData());

        const AVCodecDescriptor* desc = avcodec_descriptor_get_by_name(codec.constData());

        if (!desc)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("No codec descriptor found for %s",
                    codec.constData());

            return false;
        }

        c = avcodec_find_decoder(desc->id);
    }

    if (!c)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("No subtitle decoder found for codec: %s, try fron descriptor",
                codec.constData());

        return false;
    }

    codec_ctx = avcodec_alloc_context3(c);

    if (!codec_ctx)
        return false;

    // no way to get time base. the pts unit used in processLine() is 's', ffmpeg use ms, so set 1/1000 here

    codec_ctx->time_base.num = 1;
    codec_ctx->time_base.den = 1000;

    if (!data.isEmpty())
    {
        av_free(codec_ctx->extradata);
        codec_ctx->extradata = (uint8_t*)av_mallocz(data.size() + AV_INPUT_BUFFER_PADDING_SIZE);

        if (!codec_ctx->extradata)
            return false;

        codec_ctx->extradata_size = data.size();
        memcpy(codec_ctx->extradata, data.constData(), data.size());
    }

    if (avcodec_open2(codec_ctx, c, nullptr) < 0)
    {
        avcodec_free_context(&codec_ctx);

        return false;
    }

    return true; // codec != QByteArray("ass") && codec != QByteArray("ssa");
}

SubtitleFrame SubtitleProcessorFFmpeg::processLine(const QByteArray& data, qreal pts, qreal duration)
{
/*
    qCDebug(DIGIKAM_QTAV_LOG) << "line: " << data;
*/
    if (!codec_ctx)                                 // cppcheck-suppress identicalConditionAfterEarlyExit
    {
        return SubtitleFrame();
    }

    // AV_CODEC_ID_xxx and srt, subrip are available for ffmpeg >= 1.0. AV_CODEC_ID_xxx
    // TODO: what about other formats?
    // libav-9: packet data from demuxer contains time and but duration is 0, must decode
    // Always decode the data because it may contain styles

    if (   false && (duration > 0) && (!codec_ctx     // cppcheck-suppress identicalConditionAfterEarlyExit

#if QTAV_USE_FFMPEG(LIBAVCODEC)

        || (codec_ctx->codec_id == AV_CODEC_ID_SUBRIP)

#endif

        || (codec_ctx->codec_id == AV_CODEC_ID_SRT))
       )
    {
        SubtitleFrame f;
        f.begin = pts;
        f.end   = pts + duration;

        if (data.startsWith(QByteArray("Dialogue:")))                   // e.g. decoding embedded subtitles
            f.text = PlainText::fromAss(data.constData());
        else
            f.text = QString::fromUtf8(data.constData(), data.size());  // utf-8 is required

        return f;
    }

    AVPacket packet;
    av_init_packet(&packet);
    packet.size = data.size();
    packet.data = (uint8_t*)data.constData();

    /*
     * ffmpeg < 2.5: AVPacket.data has original text including time info, decoder use that info to get correct time
     * "Dialogue: 0,0:00:20.21,0:00:22.96,*Default,NTP,0000,0000,0000,blablabla
     * ffmpeg >= 2.5: AVPacket.data changed, we have to set correct pts & duration for packet to be decoded
     * 16,0,*Default,NTP,0000,0000,0000,,blablabla
     */

    // no codec_ctx for internal sub

    // time_base is deprecated, use framerate since 17085a0, check FF_API_AVCTX_TIMEBASE

    const double unit = 1.0 / av_q2d(codec_ctx->time_base);
    packet.pts        = pts * unit;
    packet.duration   = duration * unit;
    AVSubtitle sub;
    memset(&sub, 0, sizeof(sub));
    int got_subtitle  = 0;
    int ret           = avcodec_decode_subtitle2(codec_ctx, &sub, &got_subtitle, &packet);

    if ((ret < 0) || !got_subtitle)
    {
        av_packet_unref(&packet);
        avsubtitle_free(&sub);

        return SubtitleFrame();
    }

    SubtitleFrame frame;

    // start_display_time and duration are in ms

    frame.begin = pts + qreal(sub.start_display_time) / 1000.0;
    frame.end   = pts + qreal(sub.end_display_time)   / 1000.0;

/*
    qCDebug(DIGIKAM_QTAV_LOG) << QTime(0, 0, 0).addMSecs(frame.begin*1000.0)
                              << "-" << QTime(0, 0, 0).addMSecs(frame.end*1000.0)
                              << " fmt: " << sub.format << " pts: " << m_reader.packet().pts //sub.pts
                              << " rects: " << sub.num_rects << " end: " << sub.end_display_time;
*/
    for (unsigned i = 0 ; i < sub.num_rects ; ++i)
    {
        switch (sub.rects[i]->type)
        {
            case SUBTITLE_ASS:
            {
/*
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("ass frame: %s",
                        sub.rects[i]->ass);
*/
                frame.text.append(PlainText::fromAss(sub.rects[i]->ass)).append(ushort('\n'));
                break;
            }

            case SUBTITLE_TEXT:
            {
/*
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("txt frame: %s",
                        sub.rects[i]->text);
*/
                frame.text.append(QString::fromUtf8(sub.rects[i]->text)).append(ushort('\n'));
                break;
            }

            case SUBTITLE_BITMAP:
            {
                // sub.rects[i]->w > 0 && sub.rects[i]->h > 0
/*
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("bmp sub");
*/
                frame = SubtitleFrame(); // not support bmp subtitle now
                break;
            }

            default:
            {
                break;
            }
        }
    }

    av_packet_unref(&packet);
    avsubtitle_free(&sub);

    return frame;
}

bool SubtitleProcessorFFmpeg::processSubtitle()
{
    m_frames.clear();
    int ss = m_reader.subtitleStream();

    if (ss < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("no subtitle stream found");

        return false;
    }


#ifndef HAVE_FFMPEG_VERSION5

    codec_ctx                         = m_reader.subtitleCodecContext();
    AVCodec* dec                      = avcodec_find_decoder(codec_ctx->codec_id);

#else // ffmpeg >= 5

    AVCodecParameters* const par      = m_reader.subtitleCodecContext();
    const AVCodec* dec                = avcodec_find_decoder(par->codec_id);
    codec_ctx                         = avcodec_alloc_context3(dec);
    avcodec_parameters_to_context(codec_ctx, par);

#endif

    const AVCodecDescriptor* dec_desc = avcodec_descriptor_get(codec_ctx->codec_id);

    if (!dec)
    {
        if (dec_desc)
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Failed to find subtitle codec %s", dec_desc->name);
        else
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Failed to find subtitle codec %d", codec_ctx->codec_id);

        return false;
    }

    qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("found subtitle decoder '%s'", dec_desc->name);

    // AV_CODEC_PROP_TEXT_SUB: ffmpeg >= 2.0

#ifdef AV_CODEC_PROP_TEXT_SUB

    if (dec_desc && !(dec_desc->props & AV_CODEC_PROP_TEXT_SUB))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Only text based subtitles are currently supported");

        return false;
    }

#endif

    AVDictionary* codec_opts = nullptr;
    int ret                  = avcodec_open2(codec_ctx, dec, &codec_opts);

    if (ret < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("open subtitle codec error: %s", av_err2str(ret));

        av_dict_free(&codec_opts);

        return false;
    }

    while (!m_reader.atEnd())
    {
        if (!m_reader.readPacket())
        {
            // EOF or other errors

            continue;
        }

        if (m_reader.stream() != ss)
            continue;

        const Packet pkt = m_reader.packet();

        if (!pkt.isValid())
            continue;

        SubtitleFrame frame = processLine(pkt.data, pkt.pts, pkt.duration);

        if (frame.isValid())
            m_frames.append(frame);
    }

    avcodec_close(codec_ctx);
    codec_ctx = nullptr;

    return true;
}

} // namespace QtAV
