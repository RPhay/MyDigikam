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

#ifndef QTAV_AV_ENCODER_P_H
#define QTAV_AV_ENCODER_P_H

#include "AVEncoder.h"

// Qt includes

#include <QVariant>

// Local includes

#include "AudioFormat.h"
#include "Packet.h"
#include "VideoFormat.h"
#include "AVCompat.h"

namespace QtAV
{

class DIGIKAM_EXPORT AVEncoderPrivate : public DPtrPrivate<AVEncoder>
{
public:

    AVEncoderPrivate()
      : avctx         (nullptr),
        is_open       (false),
        bit_rate      (0),
        timestamp_mode(0),
        dict          (nullptr)
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

    virtual bool open()
    {
        return true;
    }

    virtual bool close()
    {
        return true;
    }

    // used iff avctx != null

    void applyOptionsForDict();
    void applyOptionsForContext();

public:

    AVCodecContext* avctx;    // null if not avcodec. allocated in ffmpeg based encoders
    bool            is_open;
    int             bit_rate;
    int             timestamp_mode;
    QString         codec_name;
    QVariantHash    options;
    AVDictionary*   dict;     // null if not avcodec
    Packet          packet;

private:

    Q_DISABLE_COPY(AVEncoderPrivate);
};

class AudioResampler;

class Q_DECL_HIDDEN AudioEncoderPrivate : public AVEncoderPrivate
{
public:

    AudioEncoderPrivate()
        : AVEncoderPrivate(),
          resampler       (nullptr),
          frame_size      (0)
    {
        bit_rate = 64000;
    }

    virtual ~AudioEncoderPrivate()
    {
    }

    AudioResampler* resampler;
    AudioFormat     format;
    AudioFormat     format_used;

    int             frame_size; // used if avctx->frame_size == 0

private:

    Q_DISABLE_COPY(AudioEncoderPrivate);
};

class DIGIKAM_EXPORT VideoEncoderPrivate : public AVEncoderPrivate
{
public:

    VideoEncoderPrivate()
      : AVEncoderPrivate(),
        width      (0),
        height     (0),
        frame_rate (-1),
        format_used(VideoFormat::Format_Invalid),
        format     (format_used)
    {
        bit_rate = 400000;
    }

    virtual ~VideoEncoderPrivate()
    {
    }

    int                      width;
    int                      height;
    qreal                    frame_rate;
    VideoFormat::PixelFormat format_used;
    VideoFormat              format;

private:

    Q_DISABLE_COPY(VideoEncoderPrivate);
};

} // namespace QtAV

#endif // QTAV_AV_ENCODER_P_H
