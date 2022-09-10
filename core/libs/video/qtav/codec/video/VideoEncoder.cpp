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

#include "VideoEncoder.h"

// Local includes

#include "AVEncoder_p.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

namespace QtAV
{

FACTORY_DEFINE(VideoEncoder)

void VideoEncoder_RegisterAll()
{
    static bool called = false;

    if (called)
        return;

    called = true;

    // factory.h does not check whether an id is registered

    if (!VideoEncoderFactory::Instance().registeredIds().empty()) //registered on load
        return;

    extern bool RegisterVideoEncoderFFmpeg_Man();

    RegisterVideoEncoderFFmpeg_Man();
}

QStringList VideoEncoder::supportedCodecs()
{
    // should check every registered encoders

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

        if (!av_codec_is_encoder(c) || (c->type != AVMEDIA_TYPE_VIDEO))
            continue;

        codecs.append(QString::fromLatin1(c->name));
    }

    return codecs;
}

VideoEncoder::VideoEncoder(VideoEncoderPrivate& d)
    : AVEncoder(d)
{
}

QString VideoEncoder::name() const
{
    return QLatin1String(VideoEncoder::name(id()));
}

void VideoEncoder::setWidth(int value)
{
    DPTR_D(VideoEncoder);

    if (d.width == value)
        return;

    d.width = value;

    Q_EMIT widthChanged();
}

int VideoEncoder::width() const
{
    return d_func().width;
}

void VideoEncoder::setHeight(int value)
{
    DPTR_D(VideoEncoder);

    if (d.height == value)
        return;

    d.height = value;

    Q_EMIT heightChanged();
}

int VideoEncoder::height() const
{
    return d_func().height;
}

void VideoEncoder::setFrameRate(qreal value)
{
    DPTR_D(VideoEncoder);

    if (d.frame_rate == value)
        return;

    d.frame_rate = value;

    Q_EMIT frameRateChanged();
}

qreal VideoEncoder::frameRate() const
{
    return d_func().frame_rate;
}

void VideoEncoder::setPixelFormat(const VideoFormat::PixelFormat format)
{
    DPTR_D(VideoEncoder);

    if (d.format.pixelFormat() == format)
        return;

    d.format.setPixelFormat(format);
    d.format_used = format;

    Q_EMIT pixelFormatChanged();
}

VideoFormat::PixelFormat VideoEncoder::pixelFormat() const
{
    return d_func().format_used;
}

} // namespace QtAV
