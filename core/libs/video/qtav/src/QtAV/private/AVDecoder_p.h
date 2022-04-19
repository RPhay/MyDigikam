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

#ifndef QTAV_AV_DECODER_P_H
#define QTAV_AV_DECODER_P_H

// Qt includes

#include <QHash>
#include <QSharedPointer>
#include <QVector>

// Local includes

#include "QtAV_Global.h"
#include "AVCompat.h"
#include "digikam_debug.h"

namespace QtAV
{

// always define the class to avoid macro check when using it

class AVFrameBuffers
{

#if QTAV_HAVE(AVBUFREF)

    QVector<AVBufferRef*> buf;

#endif

public:
    AVFrameBuffers(AVFrame* frame)
    {
        Q_UNUSED(frame);

#if QTAV_HAVE(AVBUFREF)

        if (!frame->buf[0])
        {
            // not ref counted. duplicate data?

            return;
        }

        buf.reserve(frame->nb_extended_buf + FF_ARRAY_ELEMS(frame->buf));
        buf.resize(frame->nb_extended_buf  + FF_ARRAY_ELEMS(frame->buf));

        for (int i = 0 ; i < (int)FF_ARRAY_ELEMS(frame->buf) ; ++i)
        {
            if (!frame->buf[i]) // so not use planes + nb_extended_buf!
            {
                continue;
            }

            buf[i] = av_buffer_ref(frame->buf[i]);

            if (!buf[i])
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("av_buffer_ref(frame->buf[%d]) error", i);
            }
        }

        if (!frame->extended_buf)
        {
            return;
        }

        for (int i = 0 ; i < frame->nb_extended_buf ; ++i)
        {
            const int k = buf.size() + i - frame->nb_extended_buf;
            buf[k]      = av_buffer_ref(frame->extended_buf[i]);

            if (!buf[k])
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("av_buffer_ref(frame->extended_buf[%d]) error", i);
            }
        }

#endif

    }
    ~AVFrameBuffers()
    {

#if QTAV_HAVE(AVBUFREF)

        foreach (AVBufferRef* b, buf)
        {
            av_buffer_unref(&b);
        }

#endif

    }
};

typedef QSharedPointer<AVFrameBuffers> AVFrameBuffersRef;

class Q_AV_PRIVATE_EXPORT AVDecoderPrivate : public DPtrPrivate<AVDecoder>
{
public:

    static const char* getProfileName(AVCodecID id, int profile)
    {
        AVCodec *c = avcodec_find_decoder(id);

        if (!c)
        {
            return "Unknow";
        }

        return av_get_profile_name(c, profile);
    }

    static const char* getProfileName(const AVCodecContext* ctx)
    {
        if (ctx->codec)
        {
            return av_get_profile_name(ctx->codec, ctx->profile);
        }

        return getProfileName(ctx->codec_id, ctx->profile);
    }

    AVDecoderPrivate()
      : codec_ctx(0)
      , available(true)
      , is_open(false)
      , undecoded_size(0)
      , dict(0)
    {
        codec_ctx = avcodec_alloc_context3(NULL);
    }

    virtual ~AVDecoderPrivate()
    {
        if (dict)
        {
            av_dict_free(&dict);
        }

        if (codec_ctx)
        {
            avcodec_free_context(&codec_ctx);
        }
    }

    virtual bool open()                 { return true; }
    virtual void close()                {              }
    virtual bool enableFrameRef() const { return true; }
    void applyOptionsForDict();
    void applyOptionsForContext();

    AVCodecContext* codec_ctx;              // set once and not change
    bool            available;              // TODO: true only when context(and hw ctx) is ready
    bool            is_open;
    int             undecoded_size;
    QString         codec_name;
    QVariantHash    options;
    AVDictionary*   dict;
};

class AudioResampler;

class AudioDecoderPrivate : public AVDecoderPrivate
{
public:

    AudioDecoderPrivate();
    virtual ~AudioDecoderPrivate();

    AudioResampler *resampler;
    QByteArray      decoded;
};

class Q_AV_PRIVATE_EXPORT VideoDecoderPrivate : public AVDecoderPrivate
{
public:

    VideoDecoderPrivate()
        : AVDecoderPrivate()
    {
    }

    virtual ~VideoDecoderPrivate()
    {
    }
};

} // namespace QtAV

Q_DECLARE_METATYPE(QtAV::AVFrameBuffersRef)

#endif // QTAV_AV_DECODER_P_H
