/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "AudioDecoder.h"

// Local includes

#include "AudioResampler.h"
#include "Packet.h"
#include "AVDecoder_p.h"
#include "AVCompat.h"
#include "QtAV_mkid.h"
#include "QtAV_factory.h"
#include "QtAV_Version.h"
#include "digikam_debug.h"

namespace QtAV
{

class AudioDecoderFFmpegPrivate;

class Q_DECL_HIDDEN AudioDecoderFFmpeg : public AudioDecoder
{
    Q_OBJECT
    Q_DISABLE_COPY(AudioDecoderFFmpeg)
    DPTR_DECLARE_PRIVATE(AudioDecoderFFmpeg)
    Q_PROPERTY(QString codecName READ codecName WRITE setCodecName NOTIFY codecNameChanged)

public:

    AudioDecoderFFmpeg();

    AudioDecoderId id()             const override final;

    virtual QString description()   const override final
    {
        const int patch = QTAV_VERSION_PATCH(avcodec_version());

        return QString::fromUtf8("%1 avcodec %2.%3.%4")
                .arg((patch >= 100) ? QLatin1String("FFmpeg")
                                    : QLatin1String("Libav"))
                .arg(QTAV_VERSION_MAJOR(avcodec_version()))
                .arg(QTAV_VERSION_MINOR(avcodec_version()))
                .arg(patch);
    }

    bool decode(const Packet& packet)     override final;
    AudioFrame frame()                    override final;

private:

    AudioDecoderFFmpeg(QObject*);
};

AudioDecoderId AudioDecoderId_FFmpeg = mkid::id32base36_6<'F','F','m','p','e','g'>::value;

FACTORY_REGISTER(AudioDecoder, FFmpeg, "FFmpeg")

class Q_DECL_HIDDEN AudioDecoderFFmpegPrivate final : public AudioDecoderPrivate
{
public:

    AudioDecoderFFmpegPrivate()
        : AudioDecoderPrivate(),
          frame              (av_frame_alloc())
    {

#if !AVCODEC_STATIC_REGISTER

        avcodec_register_all();

#endif

    }

    ~AudioDecoderFFmpegPrivate()
    {
        if (frame)
        {
            av_frame_free(&frame);
            frame = nullptr;
        }
    }

    AVFrame* frame = nullptr; // set once and not change
};

AudioDecoderId AudioDecoderFFmpeg::id() const
{
    return AudioDecoderId_FFmpeg;
}

AudioDecoderFFmpeg::AudioDecoderFFmpeg()
    : AudioDecoder(*new AudioDecoderFFmpegPrivate())
{
}

bool AudioDecoderFFmpeg::decode(const Packet& packet)
{
    if (!isAvailable())
        return false;

    DPTR_D(AudioDecoderFFmpeg);

    d.decoded.clear();
    int got_frame_ptr = 0;
    int ret           = 0;

    if (packet.isEOF())
    {
        AVPacket eofpkt;
        av_init_packet(&eofpkt);
        eofpkt.data = nullptr;
        eofpkt.size = 0;

#ifndef HAVE_FFMPEG_VERSION5

        ret         = avcodec_decode_audio4(d.codec_ctx,
                                            d.frame,
                                            &got_frame_ptr,
                                            &eofpkt);

#else // ffmpeg >= 5

        ret = avcodec_receive_frame(d.codec_ctx, d.frame);

        if      (ret == AVERROR(EAGAIN))
        {
            return false;
        }
        else if (ret < 0)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("[AudioDecoder] %s", av_err2str(ret));

            return false;
        }

        got_frame_ptr = (ret == 0);
        ret           = avcodec_send_packet(d.codec_ctx, &eofpkt);

#endif

    }
    else
    {

#ifndef HAVE_FFMPEG_VERSION5

        // const AVPacket*: ffmpeg >= 1.0. not libav

        ret = avcodec_decode_audio4(d.codec_ctx,
                                    d.frame,
                                    &got_frame_ptr,
                                    const_cast<AVPacket*>(packet.asAVPacket()));

#else // ffmpeg >= 5

    ret = avcodec_receive_frame(d.codec_ctx, d.frame);

    if      (ret == AVERROR(EAGAIN))
    {
        return false;
    }
    else if (ret < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("[AudioDecoder] %s", av_err2str(ret));

        return false;
    }

    got_frame_ptr = (ret == 0);
    ret           = avcodec_send_packet(d.codec_ctx, (AVPacket*)packet.asAVPacket());

#endif

    }

    d.undecoded_size = qMin(packet.data.size() - ret, packet.data.size());

    if (ret == AVERROR(EAGAIN))
    {
        return false;
    }

    if (ret < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("[AudioDecoder] %s",
                av_err2str(ret));

        return false;
    }

    if (!got_frame_ptr)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("[AudioDecoder] got_frame_ptr=false. decoded: %d, un: %d %s",
                ret, d.undecoded_size, av_err2str(ret));

        return !packet.isEOF();
    }

#if USE_AUDIO_FRAME

    return true;

#endif

    d.resampler->setInSampesPerChannel(d.frame->nb_samples);

    if (!d.resampler->convert((const quint8**)d.frame->extended_data))
    {
        return false;
    }

    d.decoded = d.resampler->outData();

    return true;  // return !d.decoded.isEmpty();
}

AudioFrame AudioDecoderFFmpeg::frame()
{
    DPTR_D(AudioDecoderFFmpeg);

    AudioFormat fmt;
    fmt.setSampleFormatFFmpeg(d.frame->format);
    fmt.setChannelLayoutFFmpeg(d.frame->channel_layout);
    fmt.setSampleRate(d.frame->sample_rate);

    if (!fmt.isValid())
    {
        // need more data to decode to get a frame

        return AudioFrame();
    }

    AudioFrame f(fmt);

    // av_frame_get_pkt_duration ffmpeg

    f.setBits(d.frame->extended_data);              // TODO: ref
    f.setBytesPerLine(d.frame->linesize[0], 0);     // for correct alignment
    f.setSamplesPerChannel(d.frame->nb_samples);

    // TODO: ffplay check AVFrame.pts, pkt_pts, last_pts+nb_samples. move to AudioFrame::from(AVFrame*)

#ifndef HAVE_FFMPEG_VERSION5

    f.setTimestamp((double)d.frame->pkt_pts / 1000.0);

#else // ffmpeg >= 5

    f.setTimestamp((double)d.frame->pts / 1000.0);

#endif

    f.setAudioResampler(d.resampler);               // TODO: remove. it's not safe if frame is shared. use a pool or detach if ref >1

    return f;
}

} // namespace QtAV

#include "AudioDecoderFFmpeg.moc"
