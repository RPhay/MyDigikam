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

#include "AudioResampler.h"
#include "AudioFormat.h"
#include "private/AudioResampler_p.h"
#include "private/factory.h"
#include "private/mkid.h"
#include "digikam_debug.h"

namespace QtAV
{
FACTORY_DEFINE(AudioResampler)

AudioResamplerId AudioResamplerId_FF = mkid::id32base36_6<'F', 'F', 'm', 'p', 'e', 'g'>::value;
AudioResamplerId AudioResamplerId_Libav = mkid::id32base36_5<'L', 'i', 'b', 'a', 'v'>::value;

extern bool RegisterAudioResamplerFF_Man();
extern bool RegisterAudioResamplerLibav_Man();
void AudioResampler::registerAll()
{
    static bool done = false;
    if (done)
        return;
    done = true;
#if QTAV_HAVE(SWRESAMPLE)
    RegisterAudioResamplerFF_Man();
#endif
#if QTAV_HAVE(AVRESAMPLE)
    RegisterAudioResamplerLibav_Man();
#endif
}

AudioResampler::AudioResampler(AudioResamplerPrivate& d):DPTR_INIT(&d)
{
}

AudioResampler::~AudioResampler()
{
}

QByteArray AudioResampler::outData() const
{
    return d_func().data_out;
}

bool AudioResampler::prepare()
{
    if (!inAudioFormat().isValid()) {
        qCWarning(DIGIKAM_QTAV_LOG_WARN) << QString::asprintf("src audio parameters 'channel layout(or channels), sample rate and sample format must be set before initialize resampler");
        return false;
    }
    return true;
}

bool AudioResampler::convert(const quint8 **data)
{
    Q_UNUSED(data);
    return false;
}

void AudioResampler::setSpeed(qreal speed)
{
    DPTR_D(AudioResampler);
    if (d.speed == speed)
        return;
    d.speed = speed;
    prepare();
}

qreal AudioResampler::speed() const
{
    return d_func().speed;
}


void AudioResampler::setInAudioFormat(const AudioFormat& format)
{
    DPTR_D(AudioResampler);
    if (d.in_format == format)
        return;
    d.in_format = format;
    prepare();
}

AudioFormat& AudioResampler::inAudioFormat()
{
    return d_func().in_format;
}

const AudioFormat& AudioResampler::inAudioFormat() const
{
    return d_func().in_format;
}

void AudioResampler::setOutAudioFormat(const AudioFormat& format)
{
    DPTR_D(AudioResampler);
    if (d.out_format == format)
        return;
    d.out_format = format;
    prepare();
}

AudioFormat& AudioResampler::outAudioFormat()
{
    return d_func().out_format;
}

const AudioFormat& AudioResampler::outAudioFormat() const
{
    return d_func().out_format;
}

void AudioResampler::setInSampesPerChannel(int samples)
{
    d_func().in_samples_per_channel = samples;
}

int AudioResampler::outSamplesPerChannel() const
{
    return d_func().out_samples_per_channel;
}

//channel count can be computed by av_get_channel_layout_nb_channels(chl)
void AudioResampler::setInSampleRate(int isr)
{
    AudioFormat af(d_func().in_format);
    af.setSampleRate(isr);
    setInAudioFormat(af);
}

void AudioResampler::setOutSampleRate(int osr)
{
    AudioFormat af(d_func().out_format);
    af.setSampleRate(osr);
    setOutAudioFormat(af);
}

void AudioResampler::setInSampleFormat(int isf)
{
    AudioFormat af(d_func().in_format);
    af.setSampleFormatFFmpeg(isf);
    setInAudioFormat(af);
}

void AudioResampler::setOutSampleFormat(int osf)
{
    AudioFormat af(d_func().out_format);
    af.setSampleFormatFFmpeg(osf);
    setOutAudioFormat(af);
}

void AudioResampler::setInChannelLayout(qint64 icl)
{
    AudioFormat af(d_func().in_format);
    af.setChannelLayoutFFmpeg(icl);
    setInAudioFormat(af);
}

void AudioResampler::setOutChannelLayout(qint64 ocl)
{
    AudioFormat af(d_func().out_format);
    af.setChannelLayoutFFmpeg(ocl);
    setOutAudioFormat(af);
}

void AudioResampler::setInChannels(int channels)
{
    AudioFormat af(d_func().in_format);
    af.setChannels(channels);
    setInAudioFormat(af);
}

void AudioResampler::setOutChannels(int channels)
{
    AudioFormat af(d_func().out_format);
    af.setChannels(channels);
    setOutAudioFormat(af);
}

} // namespace QtAV
