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

// Loca includes

#include "AVEncoder_p.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

namespace QtAV
{

FACTORY_DEFINE(AudioEncoder)

void AudioEncoder_RegisterAll()
{
    static bool called = false;

    if (called)
        return;

    called = true;

    // factory.h does not check whether an id is registered

    if (AudioEncoder::id("FFmpeg")) // registered on load
        return;

    extern bool RegisterAudioEncoderFFmpeg_Man();

    RegisterAudioEncoderFFmpeg_Man();
}

QStringList AudioEncoder::supportedCodecs()
{
    static QStringList codecs;

    if (!codecs.isEmpty())
        return codecs;

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
        if (!av_codec_is_encoder(c) || (c->type != AVMEDIA_TYPE_AUDIO))
            continue;

        codecs.append(QString::fromLatin1(c->name));
    }

    return codecs;
}

AudioEncoder::AudioEncoder(AudioEncoderPrivate& d)
    : AVEncoder(d)
{
}

QString AudioEncoder::name() const
{
    return QLatin1String(AudioEncoder::name(id()));
}

void AudioEncoder::setAudioFormat(const AudioFormat& format)
{
    DPTR_D(AudioEncoder);

    if (d.format == format)
        return;

    d.format      = format;
    d.format_used = format;

    Q_EMIT audioFormatChanged();
}

const AudioFormat& AudioEncoder::audioFormat() const
{
    return d_func().format_used;
}

int AudioEncoder::frameSize() const
{
    return d_func().frame_size;
}

} // namespace QtAV
