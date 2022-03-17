/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-03-17
 * Description : Methods to list FFMPEG features.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "ffmpegconfighelper.h"

// FFMpeg includes

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace Digikam
{

FFMpegProperties FFMpegConfigHelper::getVideoCodecsProperties()
{
    FFMpegProperties props;

    const AVCodec* vcodec = nullptr;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(58, 10, 100)

    void* it = nullptr;

    while ((vcodec = av_codec_iterate(&it)))

#else

    avcodec_register_all();

    while ((vcodec = av_codec_next(vcodec)))

#endif

    {
        if (vcodec->type != AVMEDIA_TYPE_VIDEO)
        {
            continue;
        }

        QString name = QString::fromUtf8(vcodec->name);
        QStringList vals;

        vals << QString::fromUtf8(vcodec->long_name);
        
        if (av_codec_is_decoder(vcodec))
        {
            vals << QLatin1String("R");
        }
        else
        {
            vals << QLatin1String("X");
        }

        if (av_codec_is_encoder(vcodec))
        {
            vals << QLatin1String("W");
        }
        else
        {
            vals << QLatin1String("X");
        }

        props.insert(name, vals);
    }

    return props;
}

FFMpegProperties FFMpegConfigHelper::getAudioCodecsProperties()
{
    FFMpegProperties props;

    const AVCodec* acodec = nullptr;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(58, 10, 100)

    void* it              = nullptr;

    while ((acodec = av_codec_iterate(&it)))

#else

    avcodec_register_all();

    while ((acodec = av_codec_next(acodec)))

#endif

    {
        if (acodec->type != AVMEDIA_TYPE_AUDIO)
        {
            continue;
        }

        QString name = QString::fromUtf8(acodec->name);
        QStringList vals;

        vals << QString::fromUtf8(acodec->long_name);

        if (av_codec_is_decoder(acodec))
        {
            vals << QLatin1String("R");
        }
        else
        {
            vals << QLatin1String("X");
        }

        if (av_codec_is_encoder(acodec))
        {
            vals << QLatin1String("W");
        }
        else
        {
            vals << QLatin1String("X");
        }

        props.insert(name, vals);
    }

    return props;
}

FFMpegProperties FFMpegConfigHelper::getExtensionsProperties()
{
    FFMpegProperties props;
    QStringList ext, exts;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(58, 10, 100)

    const AVOutputFormat* outfmt = nullptr;
    void* it                     = nullptr;

    while ((outfmt = av_muxer_iterate(&it)))

#else

    av_register_all();                  // Must register all input/output formats
    AVOutputFormat* outfmt = nullptr;

    while ((outfmt = av_oformat_next(outfmt)))

#endif

    {

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)

        if (outfmt->extensions)
        {
            ext << QString::fromUtf8(outfmt->extensions).split(QLatin1Char(','), Qt::SkipEmptyParts);
        }

#else

        if (outfmt->extensions)
        {
            ext << QString::fromUtf8(outfmt->extensions).split(QLatin1Char(','), QString::SkipEmptyParts);
        }

#endif

    }

    foreach (const QString& val, ext)
    {
        exts.append(val.trimmed());
    }

    exts.removeDuplicates();

    foreach (const QString& val, exts)
    {

        outfmt = nullptr;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(58, 10, 100)

        it     = nullptr;

        while ((outfmt = av_muxer_iterate(&it)))

#else

        while ((outfmt = av_oformat_next(outfmt)))

#endif

        {
            if (QString::fromUtf8(outfmt->extensions) == val)
            {
                props.insert(val, QStringList() << QString::fromUtf8(outfmt->long_name));
                continue;
            }
        }
    }

    return props;
}

} // namespace Digikam
