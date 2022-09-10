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

#ifndef QTAV_AV_DECODER_P_H
#define QTAV_AV_DECODER_P_H

#include "AVDecoder.h"

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

class Q_DECL_HIDDEN AVFrameBuffers
{

#if QTAV_HAVE(AVBUFREF)

    QVector<AVBufferRef*> buf;

#endif

public:

    explicit AVFrameBuffers(AVFrame* const frame)
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
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("av_buffer_ref(frame->buf[%d]) error", i);
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
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("av_buffer_ref(frame->extended_buf[%d]) error", i);
            }
        }

#endif

    }

    ~AVFrameBuffers()
    {

#if QTAV_HAVE(AVBUFREF)

        Q_FOREACH (AVBufferRef* b, buf)
        {
            av_buffer_unref(&b);
        }

#endif

    }
};

// ------------------------------------------------------------------------

typedef QSharedPointer<AVFrameBuffers> AVFrameBuffersRef;

class DIGIKAM_EXPORT AVDecoderPrivate : public DPtrPrivate<AVDecoder>
{
public:

    static const char* getProfileName(AVCodecID id, int profile)
    {

#ifndef HAVE_FFMPEG_VERSION5

        AVCodec* const c       = avcodec_find_decoder(id);

#else // ffmpeg >= 5

        const AVCodec* const c = avcodec_find_decoder(id);

#endif

        if (!c)
        {
            return "Unknow";
        }

        return av_get_profile_name(c, profile);
    }

    static const char* getProfileName(const AVCodecContext* const ctx)
    {
        if (ctx->codec)
        {
            return av_get_profile_name(ctx->codec, ctx->profile);
        }

        return getProfileName(ctx->codec_id, ctx->profile);
    }

    AVDecoderPrivate()
      : codec_ctx     (nullptr),
        available     (true),
        is_open       (false),
        undecoded_size(0),
        dict          (nullptr)
    {
        codec_ctx = avcodec_alloc_context3(nullptr);
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

public:

    AVCodecContext* codec_ctx       = nullptr;    // set once and not change
    bool            available       = false;      // TODO: true only when context(and hw ctx) is ready
    bool            is_open         = false;
    int             undecoded_size  = 0;
    QString         codec_name;
    QVariantHash    options;
    AVDictionary*   dict            = nullptr;

private:

    Q_DISABLE_COPY(AVDecoderPrivate);
};

// ------------------------------------------------------------------------

class AudioResampler;

class Q_DECL_HIDDEN AudioDecoderPrivate : public AVDecoderPrivate
{
public:

    AudioDecoderPrivate();
    virtual ~AudioDecoderPrivate();

public:

    AudioResampler* resampler = nullptr;
    QByteArray      decoded;

private:

    Q_DISABLE_COPY(AudioDecoderPrivate);
};

// ------------------------------------------------------------------------

class DIGIKAM_EXPORT VideoDecoderPrivate : public AVDecoderPrivate
{
public:

    VideoDecoderPrivate()
        : AVDecoderPrivate()
    {
    }

    virtual ~VideoDecoderPrivate()
    {
    }

private:

    Q_DISABLE_COPY(VideoDecoderPrivate);
};

} // namespace QtAV

Q_DECLARE_METATYPE(QtAV::AVFrameBuffersRef)

#endif // QTAV_AV_DECODER_P_H
