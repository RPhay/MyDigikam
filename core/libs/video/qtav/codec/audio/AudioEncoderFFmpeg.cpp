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

#include "AudioEncoder.h"

// Local includes

#include "AVEncoder_p.h"
#include "AVCompat.h"
#include "QtAV_mkid.h"
#include "QtAV_factory.h"
#include "QtAV_Version.h"
#include "digikam_debug.h"

/*!
 * options (properties) are from libavcodec/options_table.h
 * enum name here must convert to lower case to fit the names in avcodec. done in AVEncoder.setOptions()
 * Don't use lower case here because the value name may be "default" in avcodec which is a keyword of C++
 */

namespace QtAV
{

class AudioEncoderFFmpegPrivate;

class Q_DECL_HIDDEN AudioEncoderFFmpeg final : public AudioEncoder
{
    Q_OBJECT

    DPTR_DECLARE_PRIVATE(AudioEncoderFFmpeg)

public:

    AudioEncoderFFmpeg();
    AudioEncoderId id() const                           override;
    bool encode(const AudioFrame& frame = AudioFrame()) override;

private:

    // Disable
    AudioEncoderFFmpeg(QObject*);
};

static const AudioEncoderId AudioEncoderId_FFmpeg = mkid::id32base36_6<'F', 'F', 'm', 'p', 'e', 'g'>::value;

FACTORY_REGISTER(AudioEncoder, FFmpeg, "FFmpeg")

class Q_DECL_HIDDEN AudioEncoderFFmpegPrivate final : public AudioEncoderPrivate
{
public:

    AudioEncoderFFmpegPrivate()
        : AudioEncoderPrivate()
    {

#ifndef HAVE_FFMPEG_VERSION5

        avcodec_register_all();

#endif

        // nullptr: codec-specific defaults won't be initialized, which may result in suboptimal default settings (this is important mainly for encoders, e.g. libx264).

        avctx = avcodec_alloc_context3(nullptr);
    }

    bool open()  override;
    bool close() override;

public:

    QByteArray buffer;

private:

    Q_DISABLE_COPY(AudioEncoderFFmpegPrivate);
};

bool AudioEncoderFFmpegPrivate::open()
{
    if (codec_name.isEmpty())
    {
        // copy ctx from muxer by copyAVCodecContext

#ifndef HAVE_FFMPEG_VERSION5

        AVCodec* const codec = avcodec_find_encoder(avctx->codec_id);

#else // ffmpeg >= 5

        const AVCodec* codec = avcodec_find_encoder(avctx->codec_id);

#endif

        AV_ENSURE_OK(avcodec_open2(avctx, codec, &dict), false);

        return true;
    }

#ifndef HAVE_FFMPEG_VERSION5

    AVCodec* codec       = avcodec_find_encoder_by_name(codec_name.toUtf8().constData());

#else // ffmpeg >= 5

    const AVCodec* codec = avcodec_find_encoder_by_name(codec_name.toUtf8().constData());

#endif

    if (!codec)
    {
        const AVCodecDescriptor* const cd = avcodec_descriptor_get_by_name(codec_name.toUtf8().constData());

        if (cd)
        {
            codec = avcodec_find_encoder(cd->id);
        }
    }

    if (!codec)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN) << "Can not find encoder for codec " << codec_name;

        return false;
    }

    if (avctx)
    {
        avcodec_free_context(&avctx);
        avctx = nullptr;
    }

    avctx       = avcodec_alloc_context3(codec);

    // reset format_used to user defined format. important to update default format if format is invalid

    format_used = format;

    if (format.sampleRate() <= 0)
    {
        if (codec->supported_samplerates)
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("use first supported sample rate: %d",
                    codec->supported_samplerates[0]);

            format_used.setSampleRate(codec->supported_samplerates[0]);
        }
        else
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("sample rate and supported sample rate are not set. use 44100");

            format_used.setSampleRate(44100);
        }
    }

    if (format.sampleFormat() == AudioFormat::SampleFormat_Unknown)
    {
        if (codec->sample_fmts)
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("use first supported sample format: %d", codec->sample_fmts[0]);

            format_used.setSampleFormatFFmpeg((int)codec->sample_fmts[0]);
        }
        else
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("sample format and supported sample format are not set. use s16");

            format_used.setSampleFormat(AudioFormat::SampleFormat_Signed16);
        }
    }

    if (format.channelLayout() == AudioFormat::ChannelLayout_Unsupported)
    {
        if (codec->channel_layouts)
        {
            char cl[128] = { 0 };
            av_get_channel_layout_string(cl, sizeof(cl), -1, codec->channel_layouts[0]); // TODO: ff version

            qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("use first supported channel layout: %s", cl);

            format_used.setChannelLayoutFFmpeg((qint64)codec->channel_layouts[0]);
        }
        else
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("channel layout and supported channel layout are not set. use stereo");

            format_used.setChannelLayout(AudioFormat::ChannelLayout_Stereo);
        }
    }

    avctx->sample_fmt            = (AVSampleFormat)format_used.sampleFormatFFmpeg();
    avctx->channel_layout        = format_used.channelLayoutFFmpeg();
    avctx->channels              = format_used.channels();
    avctx->sample_rate           = format_used.sampleRate();
    avctx->bits_per_raw_sample   = format_used.bytesPerSample() * 8;

    /// set the time base. TODO

    avctx->time_base.num         = 1;
    avctx->time_base.den         = format_used.sampleRate();
    avctx->bit_rate              = bit_rate;

    qCDebug(DIGIKAM_QTAV_LOG) << format_used;

    /** Allow the use of the experimental AAC encoder */

    avctx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    av_dict_set(&dict, "strict", "-2", 0); // aac, vorbis
    applyOptionsForContext();

    // avctx->frame_size will be set in avcodec_open2

    AV_ENSURE_OK(avcodec_open2(avctx, codec, &dict), false);

    // from mpv ao_lavc

    int pcm_hack    = 0;
    int buffer_size = 0;
    frame_size      = avctx->frame_size;

    if (frame_size <= 1)
        pcm_hack = av_get_bits_per_sample(avctx->codec_id) / 8;

    if (pcm_hack)
    {
        frame_size  = 16384; // "enough"
        buffer_size = frame_size * pcm_hack * format_used.channels() * 2 + 200;
    }
    else
    {
        buffer_size = frame_size * format_used.bytesPerSample() * format_used.channels() * 2 + 200;
    }

    if (buffer_size < AV_INPUT_BUFFER_MIN_SIZE)
        buffer_size = AV_INPUT_BUFFER_MIN_SIZE;

    buffer.resize(buffer_size);

    return true;
}

bool AudioEncoderFFmpegPrivate::close()
{
    AV_ENSURE_OK(avcodec_close(avctx), false);

    return true;
}

AudioEncoderFFmpeg::AudioEncoderFFmpeg()
    : AudioEncoder(*new AudioEncoderFFmpegPrivate())
{
}

AudioEncoderId AudioEncoderFFmpeg::id() const
{
    return AudioEncoderId_FFmpeg;
}

bool AudioEncoderFFmpeg::encode(const AudioFrame& frame)
{
    DPTR_D(AudioEncoderFFmpeg);
    AVFrame* f = nullptr;

    if (frame.isValid())
    {
        f                       = av_frame_alloc();
        const AudioFormat fmt(frame.format());
        f->format               = fmt.sampleFormatFFmpeg();
        f->channel_layout       = fmt.channelLayoutFFmpeg();

        // f->channels = fmt.channels(); //remove? not availale in libav9
        // must be (not the last frame) exactly frame_size unless CODEC_CAP_VARIABLE_FRAME_SIZE is set (frame_size==0)
        // TODO: mpv use pcmhack for avctx.frame_size==0. can we use input frame.samplesPerChannel?

        f->nb_samples           = d.frame_size;

        // f->quality = d.avctx->global_quality; // TODO
        // TODO: record last pts. mpv compute pts internally and also use playback time

        f->pts                  = int64_t(frame.timestamp() * fmt.sampleRate()); // TODO

        // pts is set in muxer

        const int nb_planes     = frame.planeCount();

        // bytes between 2 samples on a plane. TODO: add to AudioFormat? what about bytesPerFrame?

        const int sample_stride = fmt.isPlanar() ? fmt.bytesPerSample()
                                                 : fmt.bytesPerSample()*fmt.channels();

        for (int i = 0 ; i < nb_planes ; ++i)
        {
            f->linesize[i]      = f->nb_samples * sample_stride; // frame.bytesPerLine(i);
            f->extended_data[i] = (uint8_t*)frame.constBits(i);
        }
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data       = (uint8_t*)d.buffer.constData(); // nullptr
    pkt.size       = d.buffer.size(); // 0
    int got_packet = 0;

#ifndef HAVE_FFMPEG_VERSION5

    int ret        = avcodec_encode_audio2(d.avctx, &pkt, f, &got_packet);

#else // ffmpeg >= 5

    int ret        = avcodec_send_frame(d.avctx, f);
    got_packet     = (ret == 0);
    ret            = avcodec_receive_packet(d.avctx, &pkt);

#endif

    av_frame_free(&f);

    if (ret < 0)
    {
/*
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("error avcodec_encode_audio2: %s" ,av_err2str(ret));

        av_packet_unref(&pkt); // FIXME
*/
        return false;
    }

    if (!got_packet)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("no packet got");
        d.packet = Packet();

        // invalid frame means eof

        return frame.isValid();
    }

    // qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("enc avpkt.pts: %lld, dts: %lld.", pkt.pts, pkt.dts);

    d.packet = Packet::fromAVPacket(&pkt, av_q2d(d.avctx->time_base));

    // qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("enc packet.pts: %.3f, dts: %.3f.", d.packet.pts, d.packet.dts);

    return true;
}

} // namespace QtAV

#include "AudioEncoderFFmpeg.moc"
