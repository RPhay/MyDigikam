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

// FIXME: pause=>resume error

#include "AudioOutputBackend.h"

// Qt includes

#include <QQueue>
#include <QSemaphore>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

// Apple includes

#include <AudioToolbox/AudioToolbox.h>

// Local includes

#include "QtAV_mkid.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

namespace QtAV
{

static const char kName[] = "AudioToolbox";

class Q_DECL_HIDDEN AudioOutputAudioToolbox final : public AudioOutputBackend
{
public:

    AudioOutputAudioToolbox(QObject* const parent = 0);

    QString name()                                     const override
    {
        return QLatin1String(kName);
    }

    bool isSupported(AudioFormat::SampleFormat smpfmt) const override;
    bool open()                                              override;
    bool close()                                             override;
/*
    bool flush()                                             override;
*/
    BufferControl bufferControl()                      const override;
    void onCallback()                                        override;
    bool write(const QByteArray& data)                       override;
    bool play()                                              override;
    bool setVolume(qreal value)                              override;

private:

    static void outCallback(void* inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer);
    void tryPauseTimeline();

private:

    QVector<AudioQueueBufferRef> m_buffer;
    QVector<AudioQueueBufferRef> m_buffer_fill;
    AudioQueueRef                m_queue;
    AudioStreamBasicDescription  m_desc;

    bool                         m_waiting;
    QMutex                       m_mutex;
    QWaitCondition               m_cond;
    QSemaphore                   sem;
};

typedef AudioOutputAudioToolbox AudioOutputBackendAudioToolbox;

static const AudioOutputBackendId AudioOutputBackendId_AudioToolbox = mkid::id32base36_2<'A', 'T'>::value;

FACTORY_REGISTER(AudioOutputBackend, AudioToolbox, kName)

#define AT_ENSURE(FUNC, ...) AQ_RUN_CHECK(FUNC, return __VA_ARGS__)
#define AT_WARN(FUNC, ...)   AQ_RUN_CHECK(FUNC)

#define AQ_RUN_CHECK(FUNC, ...) \
    do { \
        OSStatus ret = FUNC; \
        if (ret != noErr) { \
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("AudioBackendAudioQueue Error>>> " #FUNC ": %#x", ret); \
            __VA_ARGS__; \
        } \
    } while(0)

static AudioStreamBasicDescription audioFormatToAT(const AudioFormat &format)
{
    // AudioQueue only supports interleave

    AudioStreamBasicDescription desc;
    desc.mSampleRate       = format.sampleRate();
    desc.mFormatID         = kAudioFormatLinearPCM;
    desc.mFormatFlags      = kAudioFormatFlagIsPacked; // TODO: kAudioFormatFlagIsPacked?

    if      (format.isFloat())
        desc.mFormatFlags |= kAudioFormatFlagIsFloat;
    else if (!format.isUnsigned())
        desc.mFormatFlags |= kAudioFormatFlagIsSignedInteger;

    desc.mFramesPerPacket  = 1; // FIXME:??
    desc.mChannelsPerFrame = format.channels();
    desc.mBitsPerChannel   = format.bytesPerSample()*8;
    desc.mBytesPerFrame    = format.bytesPerFrame();
    desc.mBytesPerPacket   = desc.mBytesPerFrame * desc.mFramesPerPacket;

    return desc;
}

void AudioOutputAudioToolbox::outCallback(void* inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer)
{
    Q_UNUSED(inAQ);
    AudioOutputAudioToolbox* const ao = reinterpret_cast<AudioOutputAudioToolbox*>(inUserData);

    if (ao->bufferControl() & AudioOutputBackend::CountCallback)
    {
        ao->onCallback();
    }

    QMutexLocker locker(&ao->m_mutex);

    Q_UNUSED(locker);

    ao->m_buffer_fill.append(inBuffer);

    if (ao->m_waiting)
    {
        ao->m_waiting = false;

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("wake up to fill buffer");

        ao->m_cond.wakeOne();
    }
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("callback. sem: %d, fill queue: %d",
            ao->sem.available(), ao->m_buffer_fill.size());
*/
    ao->tryPauseTimeline();
}

AudioOutputAudioToolbox::AudioOutputAudioToolbox(QObject* const parent)
    : AudioOutputBackend(AudioOutput::DeviceFeatures() | AudioOutput::SetVolume, parent),
      m_queue           (nullptr),
      m_waiting         (false)
{
    available = false;
    available = true;
}

AudioOutputBackend::BufferControl AudioOutputAudioToolbox::bufferControl() const
{
    return CountCallback; // BufferControl(Callback | PlayedCount);
}

void AudioOutputAudioToolbox::onCallback()
{
    if (bufferControl() & CountCallback)
        sem.release();
}

void AudioOutputAudioToolbox::tryPauseTimeline()
{
    // All buffers are rendered but the AudioQueue timeline continues.
    // If the next buffer sample time is earlier than AudioQueue timeline value,
    // for example resume after pause, the buffer will not be rendered

    if (sem.available() == buffer_count)
    {
        AudioTimeStamp t;
        AudioQueueGetCurrentTime(m_queue, nullptr, &t, nullptr);

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("pause audio queue timeline @%.3f (sample time)/%lld (host time)",
                t.mSampleTime, t.mHostTime);

        AT_ENSURE(AudioQueuePause(m_queue));
    }
}

bool AudioOutputAudioToolbox::isSupported(AudioFormat::SampleFormat smpfmt) const
{
    return !IsPlanar(smpfmt);
}

bool AudioOutputAudioToolbox::open()
{
    m_buffer.resize(buffer_count);
    m_desc = audioFormatToAT(format);

    AT_ENSURE(AudioQueueNewOutput(&m_desc, AudioOutputAudioToolbox::outCallback, this, nullptr, kCFRunLoopCommonModes/*nullptr*/, 0, &m_queue), false);

    for (int i = 0 ; i < m_buffer.size() ; ++i)
    {
        AT_ENSURE(AudioQueueAllocateBuffer(m_queue, buffer_size, &m_buffer[i]), false);
    }

    m_buffer_fill = m_buffer;

    sem.release(buffer_count - sem.available());
    m_waiting     = false;

    return true;
}

bool AudioOutputAudioToolbox::close()
{
    if (!m_queue)
    {
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("AudioQueue is not created. skip close");

        return true;
    }

    UInt32 running = 0;
    UInt32 s       = 0;
    AT_ENSURE(AudioQueueGetProperty(m_queue, kAudioQueueProperty_IsRunning, &running, &s), false);

    if (running)
        AT_ENSURE(AudioQueueStop(m_queue, true), false);

    // dispose all resouces including buffers, so we can remove AudioQueueFreeBuffer

    AT_ENSURE(AudioQueueDispose(m_queue, true), false);
    m_queue = nullptr;
    m_buffer.clear();

    return true;
}

bool AudioOutputAudioToolbox::write(const QByteArray& data)
{
    // blocking queue.
    // if queue not full, fill buffer and enqueue buffer
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("write. sem: %d", sem.available());
*/
    if (bufferControl() & CountCallback)
        sem.acquire();

    AudioQueueBufferRef buf = nullptr;
    {
        QMutexLocker locker(&m_mutex);
        Q_UNUSED(locker);

        // put to data queue, if has buffer to fill (was available in callback), fill the front data

        if (m_buffer_fill.isEmpty())
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("buffer queue to fill is empty, wait a valid buffer to fill");

            m_waiting = true;
            m_cond.wait(&m_mutex);
        }

        buf = m_buffer_fill.first();
        m_buffer_fill.removeFirst();
    }

    assert(buf->mAudioDataBytesCapacity >= (UInt32)data.size() && "too many data to write to audio queue buffer");

    memcpy(buf->mAudioData, data.constData(), data.size());
    buf->mAudioDataByteSize = data.size();

    // buf->mUserData

    AT_ENSURE(AudioQueueEnqueueBuffer(m_queue, buf, 0, nullptr), false);

    return true;
}

bool AudioOutputAudioToolbox::play()
{
    OSType err = AudioQueueStart(m_queue, nullptr);

    if (err == '!pla')
    {
        // AVAudioSessionErrorCodeCannotStartPlaying

        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("AudioQueueStart error: AVAudioSessionErrorCodeCannotStartPlaying. "
                                 "May play in background");

        close();
        open();

        return false;
    }

    if (err != noErr)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("AudioQueueStart error: %#x", noErr);

        return false;
    }

    return true;
}

bool AudioOutputAudioToolbox::setVolume(qreal value)
{
    // iOS document says the range is [0,1]. But >1.0 works on macOS. So no manually check range here

    AT_ENSURE(AudioQueueSetParameter(m_queue, kAudioQueueParam_Volume, value), false);

    return true;
}

} // namespace QtAV
