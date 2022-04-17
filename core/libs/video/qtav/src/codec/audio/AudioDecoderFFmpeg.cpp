/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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
#include "AudioResampler.h"
#include "Packet.h"
#include "private/AVDecoder_p.h"
#include "private/AVCompat.h"
#include "private/mkid.h"
#include "private/factory.h"
#include "QtAV_Version.h"
#include "utils/Logger.h"

namespace QtAV
{

class AudioDecoderFFmpegPrivate;
class AudioDecoderFFmpeg : public AudioDecoder
{
    Q_OBJECT
    Q_DISABLE_COPY(AudioDecoderFFmpeg)
    DPTR_DECLARE_PRIVATE(AudioDecoderFFmpeg)
    Q_PROPERTY(QString codecName READ codecName WRITE setCodecName NOTIFY codecNameChanged)
public:
    AudioDecoderFFmpeg();
    AudioDecoderId id() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QString description() const Q_DECL_OVERRIDE Q_DECL_FINAL {
        const int patch = QTAV_VERSION_PATCH(avcodec_version());
        return QStringLiteral("%1 avcodec %2.%3.%4")
                .arg(patch>=100?QStringLiteral("FFmpeg"):QStringLiteral("Libav"))
                .arg(QTAV_VERSION_MAJOR(avcodec_version())).arg(QTAV_VERSION_MINOR(avcodec_version())).arg(patch);
    }
    bool decode(const Packet& packet) Q_DECL_OVERRIDE Q_DECL_FINAL;
    AudioFrame frame() Q_DECL_OVERRIDE Q_DECL_FINAL;
Q_SIGNALS:
    void codecNameChanged() Q_DECL_OVERRIDE Q_DECL_FINAL;
};

AudioDecoderId AudioDecoderId_FFmpeg = mkid::id32base36_6<'F','F','m','p','e','g'>::value;
FACTORY_REGISTER(AudioDecoder, FFmpeg, "FFmpeg")

class AudioDecoderFFmpegPrivate Q_DECL_FINAL: public AudioDecoderPrivate
{
public:
    AudioDecoderFFmpegPrivate()
        : AudioDecoderPrivate()
        , frame(av_frame_alloc())
    {
#if !AVCODEC_STATIC_REGISTER
        avcodec_register_all();
#endif
    }
    ~AudioDecoderFFmpegPrivate() {
        if (frame) {
            av_frame_free(&frame);
            frame = 0;
        }
    }

    AVFrame *frame; //set once and not change
};

AudioDecoderId AudioDecoderFFmpeg::id() const
{
    return AudioDecoderId_FFmpeg;
}

AudioDecoderFFmpeg::AudioDecoderFFmpeg()
    : AudioDecoder(*new AudioDecoderFFmpegPrivate())
{
}

bool AudioDecoderFFmpeg::decode(const Packet &packet)
{
    if (!isAvailable())
        return false;
    DPTR_D(AudioDecoderFFmpeg);
    d.decoded.clear();
    int got_frame_ptr = 0;
    int ret = 0;
    if (packet.isEOF()) {
        AVPacket eofpkt;
        av_init_packet(&eofpkt);
        eofpkt.data = NULL;
        eofpkt.size = 0;
        ret = avcodec_decode_audio4(d.codec_ctx, d.frame, &got_frame_ptr, &eofpkt);
    } else {
    // const AVPacket*: ffmpeg >= 1.0. no libav
        ret = avcodec_decode_audio4(d.codec_ctx, d.frame, &got_frame_ptr, (AVPacket*)packet.asAVPacket());
    }
    d.undecoded_size = qMin(packet.data.size() - ret, packet.data.size());
    if (ret == AVERROR(EAGAIN)) {
        return false;
    }
    if (ret < 0) {
        qWarning("[AudioDecoder] %s", av_err2str(ret));
        return false;
    }
    if (!got_frame_ptr) {
        qWarning("[AudioDecoder] got_frame_ptr=false. decoded: %d, un: %d %s", ret, d.undecoded_size, av_err2str(ret));
        return !packet.isEOF();
    }
#if USE_AUDIO_FRAME
    return true;
#endif
    d.resampler->setInSampesPerChannel(d.frame->nb_samples);
    if (!d.resampler->convert((const quint8**)d.frame->extended_data)) {
        return false;
    }
    d.decoded = d.resampler->outData();
    return true;
    return !d.decoded.isEmpty();
}

AudioFrame AudioDecoderFFmpeg::frame()
{
    DPTR_D(AudioDecoderFFmpeg);
    AudioFormat fmt;
    fmt.setSampleFormatFFmpeg(d.frame->format);
    fmt.setChannelLayoutFFmpeg(d.frame->channel_layout);
    fmt.setSampleRate(d.frame->sample_rate);
    if (!fmt.isValid()) {// need more data to decode to get a frame
        return AudioFrame();
    }
    AudioFrame f(fmt);
    //av_frame_get_pkt_duration ffmpeg
    f.setBits(d.frame->extended_data); // TODO: ref
    f.setBytesPerLine(d.frame->linesize[0], 0); // for correct alignment
    f.setSamplesPerChannel(d.frame->nb_samples);
    // TODO: ffplay check AVFrame.pts, pkt_pts, last_pts+nb_samples. move to AudioFrame::from(AVFrame*)
    f.setTimestamp((double)d.frame->pkt_pts/1000.0);
    f.setAudioResampler(d.resampler); // TODO: remove. it's not safe if frame is shared. use a pool or detach if ref >1
    return f;
}

} // namespace QtAV

#include "AudioDecoderFFmpeg.moc"
