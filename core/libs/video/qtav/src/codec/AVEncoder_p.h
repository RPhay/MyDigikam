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

#ifndef QTAV_AV_ENCODER_P_H
#define QTAV_AV_ENCODER_P_H

// Qt includes

#include <QVariant>

// Local includes

#include "AudioFormat.h"
#include "Packet.h"
#include "VideoFormat.h"
#include "AVCompat.h"

namespace QtAV
{

class QTAV_PRIVATE_EXPORT AVEncoderPrivate : public DPtrPrivate<AVEncoder>
{
public:

    AVEncoderPrivate()
      : avctx(0)
      , is_open(false)
      , bit_rate(0)
      , timestamp_mode(0)
      , dict(0)
    {
    }

    virtual ~AVEncoderPrivate()
    {
        if (dict)
        {
            av_dict_free(&dict);
        }

        if (avctx)
        {
            avcodec_free_context(&avctx);
        }
    }

    virtual bool open()  { return true; }
    virtual bool close() { return true; }

    // used iff avctx != null

    void applyOptionsForDict();
    void applyOptionsForContext();

    AVCodecContext* avctx;    // null if not avcodec. allocated in ffmpeg based encoders
    bool            is_open;
    int             bit_rate;
    int             timestamp_mode;
    QString         codec_name;
    QVariantHash    options;
    AVDictionary*   dict;     // null if not avcodec
    Packet          packet;
};

class AudioResampler;

class AudioEncoderPrivate : public AVEncoderPrivate
{
public:

    AudioEncoderPrivate()
        : AVEncoderPrivate()
        , frame_size(0)
    {
        bit_rate = 64000;
    }

    virtual ~AudioEncoderPrivate()
    {
    }

    AudioResampler* resampler;
    AudioFormat     format, format_used;

    int             frame_size; // used if avctx->frame_size == 0
};

class QTAV_PRIVATE_EXPORT VideoEncoderPrivate : public AVEncoderPrivate
{
public:

    VideoEncoderPrivate()
      : AVEncoderPrivate()
      , width(0)
      , height(0)
      , frame_rate(-1)
      , format_used(VideoFormat::Format_Invalid)
      , format(format_used)
    {
        bit_rate = 400000;
    }

    virtual ~VideoEncoderPrivate()
    {
    }

    int                      width, height;
    qreal                    frame_rate;
    VideoFormat::PixelFormat format_used;
    VideoFormat              format;
};

} // namespace QtAV

#endif // QTAV_AV_ENCODER_P_H
