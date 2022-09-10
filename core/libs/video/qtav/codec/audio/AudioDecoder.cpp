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

#include "AudioDecoder.h"

// Local includes

#include "AVDecoder_p.h"
#include "AVCompat.h"
#include "QtAV_factory.h"
#include "AudioResampler.h"
#include "digikam_debug.h"

namespace QtAV
{

FACTORY_DEFINE(AudioDecoder)

// TODO: why vc can not declare extern func in a class member? resolved as &func@@YAXXZ

extern bool RegisterAudioDecoderFFmpeg_Man();

void AudioDecoder::registerAll()
{
    static bool done = false;

    if (done)
        return;

    done = true;
    RegisterAudioDecoderFFmpeg_Man();
}

QStringList AudioDecoder::supportedCodecs()
{
    static QStringList codecs;

    if (!codecs.isEmpty())
        return codecs;

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
        if (!av_codec_is_decoder(c) || (c->type != AVMEDIA_TYPE_AUDIO))
            continue;

        codecs.append(QString::fromLatin1(c->name));
    }

    return codecs;
}

AudioDecoderPrivate::AudioDecoderPrivate()
    : AVDecoderPrivate(),
      resampler       (nullptr)
{
    resampler = AudioResampler::create(AudioResamplerId_FF);

    if (!resampler)
        resampler = AudioResampler::create(AudioResamplerId_Libav);

    if (resampler)
        resampler->setOutSampleFormat(AV_SAMPLE_FMT_FLT);
}

AudioDecoderPrivate::~AudioDecoderPrivate()
{
    if (resampler)
    {
        delete resampler;
        resampler = nullptr;
    }
}

AudioDecoder::AudioDecoder(AudioDecoderPrivate& d)
    : AVDecoder(d)
{
}

QString AudioDecoder::name() const
{
    return QLatin1String(AudioDecoder::name(id()));
}

QByteArray AudioDecoder::data() const
{
    return d_func().decoded;
}

AudioResampler* AudioDecoder::resampler()
{
    return d_func().resampler;
}

} // namespace QtAV
