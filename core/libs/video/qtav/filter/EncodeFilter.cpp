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

#include "EncodeFilter.h"

// C++ includes

#include <limits>

// Qt includes

#include <QAtomicInt>
#include <QCoreApplication>
#include <QThread>

// Local includes

#include "Filter_p.h"
#include "AudioEncoder.h"
#include "VideoEncoder.h"
#include "digikam_debug.h"

namespace QtAV
{

class Q_DECL_HIDDEN AudioEncodeFilterPrivate final : public AudioFilterPrivate
{
public:

    AudioEncodeFilterPrivate()
        : enc          (nullptr),
          start_time   (0),
          async        (false),
          finishing    (0),
          leftOverAudio()
    {
    }

    ~AudioEncodeFilterPrivate()
    {
        if (enc)
        {
            enc->close();
            delete enc;
        }
    }

    AudioEncoder*   enc;
    qint64          start_time;
    bool            async;
    QAtomicInt      finishing;
    QThread         enc_thread;
    AudioFrame      leftOverAudio;
};

AudioEncodeFilter::AudioEncodeFilter(QObject* const parent)
    : AudioFilter(*new AudioEncodeFilterPrivate(), parent)
{
    connect(this, &AudioEncodeFilter::requestToEncode,
            this, &AudioEncodeFilter::encode);

    connect(this, SIGNAL(finished()),
            &d_func().enc_thread, SLOT(quit()));
}

void AudioEncodeFilter::setAsync(bool value)
{
    DPTR_D(AudioEncodeFilter);

    if (d.async == value)
        return;

    if (value)
        moveToThread(&d.enc_thread);
    else
        moveToThread(qApp->thread());

    d.async = value;
}

bool AudioEncodeFilter::isAsync() const
{
    return d_func().async;
}

AudioEncoder* AudioEncodeFilter::createEncoder(const QString& name)
{
    DPTR_D(AudioEncodeFilter);

    if (d.enc)
    {
        d.enc->close();
        delete d.enc;
    }

    d.enc = AudioEncoder::create(name.toLatin1().constData());

    return d.enc;
}

AudioEncoder* AudioEncodeFilter::encoder() const
{
    return d_func().enc;
}

qint64 AudioEncodeFilter::startTime() const
{
    return d_func().start_time;
}

void AudioEncodeFilter::setStartTime(qint64 value)
{
    DPTR_D(AudioEncodeFilter);

    if (d.start_time == value)
        return;

    d.start_time = value;

    Q_EMIT startTimeChanged(value);
}

void AudioEncodeFilter::finish()
{
    DPTR_D(AudioEncodeFilter);

    if (isAsync() && !d.enc_thread.isRunning())
        return;

    if (!d.finishing.testAndSetRelaxed(0, 1))
        return;

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("About finish audio encoding");

    AudioFrame f;
    f.setTimestamp(std::numeric_limits<qreal>::max());

    if (isAsync())
    {
        Q_EMIT requestToEncode(f);
    }
    else
    {
        encode(f); // FIXME: not thread safe. lock in encode?
    }
}

void AudioEncodeFilter::process(Statistics* statistics, AudioFrame* frame)
{
    Q_UNUSED(statistics);

    DPTR_D(AudioEncodeFilter);

    if (!isAsync())
    {
        encode(*frame);
        return;
    }

    if (!d.enc_thread.isRunning())
        d.enc_thread.start();

    Q_EMIT requestToEncode(*frame);
}

void AudioEncodeFilter::encode(const AudioFrame& frame)
{
    DPTR_D(AudioEncodeFilter);

    if (!d.enc)
        return;

    // encode delayed frames can pass an invalid frame

    if (!d.enc->isOpen() && frame.isValid())
    {

#if 0 // TODO: if set the input format, check whether it is supported in open()

        if (!d.enc->audioFormat().isValid())
        {
            AudioFormat af(frame.format());
/*
            if (af.isPlanar())
                af.setSampleFormat(AudioFormat::packedSampleFormat(af.sampleFormat()));
*/
            af.setSampleFormat(AudioFormat::SampleFormat_Unknown);
            d.enc->setAudioFormat(af);
        }

#endif

        if (!d.enc->open())
        {
            // TODO: error()

            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Failed to open audio encoder");

            return;
        }

        Q_EMIT readyToEncode();
    }

    if (!frame.isValid() && (frame.timestamp() == std::numeric_limits<qreal>::max()))
    {
        while (d.enc->encode())
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("encode delayed audio frames...");

            Q_EMIT frameEncoded(d.enc->encoded());
        }

        d.enc->close();

        Q_EMIT finished();

        d.finishing = 0;

        return;
    }

    if ((frame.timestamp() * 1000.0) < startTime())
        return;

    AudioFrame f(frame);

    if (f.format() != d.enc->audioFormat())
        f = f.to(d.enc->audioFormat());

    if (d.leftOverAudio.isValid())
    {
        f.prepend(d.leftOverAudio);
        d.leftOverAudio = AudioFrame();
    }

    int frameSizeEncoder = d.enc->frameSize() ? d.enc->frameSize() : f.samplesPerChannel();
    int frameSize        = f.samplesPerChannel();

    QList<AudioFrame> audioFrames;

    for (int i = 0 ; i < frameSize ; i += frameSizeEncoder)
    {
        if ((frameSize - i) >= frameSizeEncoder)
        {
            audioFrames.append(f.mid(i, frameSizeEncoder));
        }
        else
        {
            d.leftOverAudio = f.mid(i);
        }
    }

    for (int i = 0 ; i < audioFrames.length() ; ++i)
    {
        if (!d.enc->encode(audioFrames.at(i)))
        {
            if (f.timestamp() == std::numeric_limits<qreal>::max())
            {
                Q_EMIT finished();

                d.finishing = 0;
            }

            return;
        }

        if (!d.enc->encoded().isValid())
            return;

        Q_EMIT frameEncoded(d.enc->encoded());
    }
}

class Q_DECL_HIDDEN VideoEncodeFilterPrivate final : public VideoFilterPrivate
{
public:

    VideoEncodeFilterPrivate()
        : enc       (nullptr),
          start_time(0),
          async     (false),
          finishing (0)
    {
    }

    ~VideoEncodeFilterPrivate()
    {
        if (enc)
        {
            enc->close();
            delete enc;
        }
    }

    VideoEncoder*   enc;
    qint64          start_time;
    bool            async;
    QAtomicInt      finishing;
    QThread         enc_thread;
};

VideoEncodeFilter::VideoEncodeFilter(QObject* const parent)
    : VideoFilter(*new VideoEncodeFilterPrivate(), parent)
{
    connect(this, &VideoEncodeFilter::requestToEncode,
            this, &VideoEncodeFilter::encode);

    connect(this, SIGNAL(finished()),
            &d_func().enc_thread, SLOT(quit()));
}

void VideoEncodeFilter::setAsync(bool value)
{
    DPTR_D(VideoEncodeFilter);

    if (d.async == value)
        return;

    if (value)
        moveToThread(&d.enc_thread);
    else
        moveToThread(qApp->thread()); // if async but not in main thread, queued sig/slot connection will not work

    d.async = value;
}

bool VideoEncodeFilter::isAsync() const
{
    return d_func().async;
}

VideoEncoder* VideoEncodeFilter::createEncoder(const QString& name)
{
    DPTR_D(VideoEncodeFilter);

    if (d.enc)
    {
        d.enc->close();
        delete d.enc;
    }

    d.enc = VideoEncoder::create(name.toLatin1().constData());

    return d.enc;
}

VideoEncoder* VideoEncodeFilter::encoder() const
{
    return d_func().enc;
}

qint64 VideoEncodeFilter::startTime() const
{
    return d_func().start_time;
}

void VideoEncodeFilter::setStartTime(qint64 value)
{
    DPTR_D(VideoEncodeFilter);

    if (d.start_time == value)
        return;

    d.start_time = value;

    Q_EMIT startTimeChanged(value);
}

void VideoEncodeFilter::finish()
{
    DPTR_D(VideoEncodeFilter);

    if (isAsync() && !d.enc_thread.isRunning())
        return;

    if (!d.finishing.testAndSetRelaxed(0, 1))
        return;

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("About finish video encoding");

    VideoFrame f;
    f.setTimestamp(std::numeric_limits<qreal>::max());

    if (isAsync())
    {
        Q_EMIT requestToEncode(f);
    }
    else
    {
        encode(f);
    }
}

void VideoEncodeFilter::process(Statistics* statistics, VideoFrame* frame)
{
    Q_UNUSED(statistics);

    DPTR_D(VideoEncodeFilter);

    if (!isAsync())
    {
        encode(*frame);

        return;
    }

    if (!d.enc_thread.isRunning())
        d.enc_thread.start();

    Q_EMIT requestToEncode(*frame);
}

void VideoEncodeFilter::encode(const VideoFrame& frame)
{
    DPTR_D(VideoEncodeFilter);

    if (!d.enc)
        return;

    // encode delayed frames can pass an invalid frame

    if (!d.enc->isOpen() && frame.isValid())
    {
        if (d.enc->width() == 0)
        {
            d.enc->setWidth(frame.width());
        }

        if (d.enc->height() == 0)
        {
            d.enc->setHeight(frame.height());
        }

        if (!d.enc->open())
        {
            // TODO: error()

            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Failed to open video encoder");

            return;
        }

        Q_EMIT readyToEncode();
    }

    if (!frame.isValid() && (frame.timestamp() == std::numeric_limits<qreal>::max()))
    {
        while (d.enc->encode())
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("encode delayed video frames...");

            Q_EMIT frameEncoded(d.enc->encoded());
        }

        d.enc->close();

        Q_EMIT finished();

        d.finishing = 0;

        return;
    }

    if ((frame.timestamp()*1000.0) < startTime())
        return;

    // TODO: async

    VideoFrame f(frame);

    if (
        (f.pixelFormat() != d.enc->pixelFormat()) ||
        (d.enc->width()  != f.width())            ||
        (d.enc->height() != f.height())
       )
    {
        f = f.to(d.enc->pixelFormat(), QSize(d.enc->width(), d.enc->height()));
    }

    if (!d.enc->encode(f))
    {
        if (f.timestamp() == std::numeric_limits<qreal>::max())
        {
            Q_EMIT finished();

            d.finishing = 0;
        }

        return;
    }

    if (!d.enc->encoded().isValid())
        return;

    Q_EMIT frameEncoded(d.enc->encoded());
}

} // namespace QtAV
