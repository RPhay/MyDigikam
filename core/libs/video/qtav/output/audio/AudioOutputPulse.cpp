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

#include "AudioOutputBackend.h"

// Qt includes

#include <QCoreApplication>
#include <QMetaObject>

// Pulse includes

#include <pulse/pulseaudio.h>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "QtAV_mkid.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

#ifndef Q_LIKELY
#   define Q_LIKELY(x) (!!(x))
#endif

namespace QtAV
{

static const char kName[] = "Pulse";

class Q_DECL_HIDDEN AudioOutputPulse final : public AudioOutputBackend
{
    Q_OBJECT

public:

    AudioOutputPulse(QObject* const parent = nullptr);

    QString name()                                                       const final
    {
        return QString::fromLatin1(kName);
    }

    bool isSampleFormatSupported(AudioFormat::SampleFormat sampleFormat) const;
    bool open()                                                                final;
    bool close()                                                               final;

protected:

    bool write(const QByteArray& data)                                         final;
    bool play()                                                                final;
    BufferControl bufferControl()                                        const final;
    int getWritableBytes()                                                     final;

    bool setVolume(qreal value)                                                final;
    qreal getVolume()                                                    const final;
    bool setMute(bool value = true)                                            final;

private:

    bool init(const AudioFormat& format);
    static void contextStateCallback(pa_context* c, void* userdata);
    static void contextSubscribeCallback(pa_context* c, pa_subscription_event_type_t t, uint32_t idx, void* userdata);
    static void stateCallback(pa_stream* s, void* userdata);
    static void latencyUpdateCallback(pa_stream* s, void* userdata);
    static void underflowCallback(pa_stream* s, void* userdata);
    static void writeCallback(pa_stream* s, size_t length, void* userdata);
    static void  successCallback(pa_stream* s, int success, void* userdata);
    static void sinkInfoCallback(struct pa_context* c, const struct pa_sink_input_info* i, int is_last, void* userdata);

    bool waitPAOperation(pa_operation* op) const
    {
        if (!op)
        {
            return false;
        }

        pa_operation_state_t state = pa_operation_get_state(op);

        while (state == PA_OPERATION_RUNNING)
        {
            pa_threaded_mainloop_wait(loop);
            state = pa_operation_get_state(op);
        }

        pa_operation_unref(op);

        return (state == PA_OPERATION_DONE);
    }

private:

    pa_threaded_mainloop*   loop    = nullptr;
    pa_context*             ctx     = nullptr;
    pa_stream*              stream  = nullptr;
    pa_sink_input_info      info;
    size_t                  writable_size; // has the same effect as pa_stream_writable_size
};

typedef AudioOutputPulse AudioOutputBackendPulse;

static const AudioOutputBackendId AudioOutputBackendId_Pulse = mkid::id32base36_5<'P', 'u', 'l', 's', 'e'>::value;

FACTORY_REGISTER(AudioOutputBackend, Pulse, kName)

#define PA_ENSURE_TRUE(expr, ...) \
    do { \
        if (!(expr)) { \
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("PulseAudio error @%d " #expr ": %s", __LINE__, pa_strerror(pa_context_errno(ctx))); \
            return __VA_ARGS__; \
        } \
    } while(0)

static const struct Q_DECL_HIDDEN format_entry
{
    AudioFormat::SampleFormat spformat;
    pa_sample_format_t        pa;
}
format_map[] =
{
    { AudioFormat::SampleFormat_Signed16,  PA_SAMPLE_S16NE     },
    { AudioFormat::SampleFormat_Signed32,  PA_SAMPLE_S32NE     },
    { AudioFormat::SampleFormat_Float,     PA_SAMPLE_FLOAT32NE },
    { AudioFormat::SampleFormat_Unsigned8, PA_SAMPLE_U8        },
    { AudioFormat::SampleFormat_Unknown,   PA_SAMPLE_INVALID   }
};

AudioFormat::SampleFormat sampleFormatFromPulse(pa_sample_format pa)
{
    for (int i = 0 ; format_map[i].spformat != AudioFormat::SampleFormat_Unknown ; ++i)
    {
        if (format_map[i].pa == pa)
            return format_map[i].spformat;
    }

    return AudioFormat::SampleFormat_Unknown;
}

static pa_sample_format sampleFormatToPulse(AudioFormat::SampleFormat format)
{
    for (int i = 0 ; format_map[i].spformat != AudioFormat::SampleFormat_Unknown ; ++i)
    {
        if (format_map[i].spformat == format)
            return format_map[i].pa;
    }

    return PA_SAMPLE_INVALID;
}

class Q_DECL_HIDDEN ScopedPALocker
{
    pa_threaded_mainloop* ml = nullptr;

public:

    explicit ScopedPALocker(pa_threaded_mainloop* const loop)
        : ml(loop)
    {
        pa_threaded_mainloop_lock(ml);
    }

    ~ScopedPALocker()
    {
        pa_threaded_mainloop_unlock(ml);
    }
};

void AudioOutputPulse::contextStateCallback(pa_context* c, void* userdata)
{
    AudioOutputPulse* const p = reinterpret_cast<AudioOutputPulse*>(userdata);

    switch (pa_context_get_state(c))
    {
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
        case PA_CONTEXT_READY:
        {
            pa_threaded_mainloop_signal(p->loop, 0);

            break;
        }

        default:
        {
            break;
        }
    }
}

static void sink_input_info_cb(pa_context* c, const pa_sink_input_info* i, int eol, void* userdata)
{
    Q_UNUSED(c);

    if (eol)
        return;

    AudioOutputPulse* const ao = reinterpret_cast<AudioOutputPulse*>(userdata);
    QMetaObject::invokeMethod(ao, "volumeReported", Q_ARG(qreal, (qreal)pa_cvolume_avg(&i->volume) / qreal(PA_VOLUME_NORM)));
    QMetaObject::invokeMethod(ao, "muteReported",   Q_ARG(bool, i->mute));
}

static void sink_input_event(pa_context* c, pa_subscription_event_type_t t, uint32_t idx, AudioOutputPulse* ao)
{
    switch (t)
    {
        case PA_SUBSCRIPTION_EVENT_REMOVE:
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("PulseAudio sink killed");

            break;
        }

        default:
        {
            pa_operation* const op = pa_context_get_sink_input_info(c, idx, sink_input_info_cb, ao);

            if (Q_LIKELY(!!op))
                pa_operation_unref(op);

            break;
        }
    }
}

void AudioOutputPulse::contextSubscribeCallback(pa_context* c,
                                                pa_subscription_event_type_t type,
                                                uint32_t idx,
                                                void* userdata)
{
    AudioOutputPulse* const p      = reinterpret_cast<AudioOutputPulse*>(userdata);
    unsigned facility              = type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
    pa_subscription_event_type_t t = pa_subscription_event_type_t(type & PA_SUBSCRIPTION_EVENT_TYPE_MASK);

    switch (facility)
    {
        case PA_SUBSCRIPTION_EVENT_SINK:
        {
            break;
        }

        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
        {
            if (p->stream && (idx == pa_stream_get_index(p->stream)))
                sink_input_event(c, t, idx, p);

            break;
        }

        case PA_SUBSCRIPTION_EVENT_CARD:
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("PA_SUBSCRIPTION_EVENT_CARD");

            break;
        }

        default:
        {
            break;
        }
    }
}

void AudioOutputPulse::stateCallback(pa_stream* s, void* userdata)
{
    AudioOutputPulse* const p = reinterpret_cast<AudioOutputPulse*>(userdata);

    switch (pa_stream_get_state(s))
    {
        case PA_STREAM_FAILED:
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("PA_STREAM_FAILED");

            pa_threaded_mainloop_signal(p->loop, 0);

            break;
        }

        case PA_STREAM_READY:
        case PA_STREAM_TERMINATED:
        {
            pa_threaded_mainloop_signal(p->loop, 0);

            break;
        }

        default:
        {
            break;
        }
    }
}

void AudioOutputPulse::latencyUpdateCallback(pa_stream* s, void* userdata)
{
    Q_UNUSED(s);
    AudioOutputPulse* const p = reinterpret_cast<AudioOutputPulse*>(userdata);
    pa_threaded_mainloop_signal(p->loop, 0);
}

void AudioOutputPulse::writeCallback(pa_stream* s, size_t length, void* userdata)
{
    Q_UNUSED(s);

    // length: writable bytes. callback is called pirioddically

    AudioOutputPulse* const p = reinterpret_cast<AudioOutputPulse*>(userdata);
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("write callback: %d + %d", p->writable_size, length);
*/
    p->writable_size          = length;
    p->onCallback();
}

void AudioOutputPulse::successCallback(pa_stream* s, int success, void* userdata)
{
    Q_UNUSED(s);
    Q_UNUSED(success); // ?

    AudioOutputPulse* const p = reinterpret_cast<AudioOutputPulse*>(userdata);
    pa_threaded_mainloop_signal(p->loop, 0);
}

void AudioOutputPulse::sinkInfoCallback(pa_context* c,
                                        const pa_sink_input_info* i,
                                        int is_last,
                                        void* userdata)
{
    Q_UNUSED(c);
    AudioOutputPulse* const p = reinterpret_cast<AudioOutputPulse*>(userdata);

    if (is_last < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Failed to get sink input info");

        return;
    }

    if (!i)
        return;

    p->info = *i;
    pa_threaded_mainloop_signal(p->loop, 0);
}

bool AudioOutputPulse::init(const AudioFormat& format)
{
    writable_size = 0;
    loop          = pa_threaded_mainloop_new();

    if (pa_threaded_mainloop_start(loop) < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("PulseAudio failed to start mainloop");

        return false;
    }

    ScopedPALocker lock(loop);

    Q_UNUSED(lock);

    pa_mainloop_api* const api = pa_threaded_mainloop_get_api(loop);
    ctx                        = pa_context_new(api,
                                                qApp->applicationName()
                                                    .append(QLatin1String(" @%1 (QtAV)"))
                                                    .arg((quintptr)this).toUtf8().constData());

    if (!ctx)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("PulseAudio failed to allocate a context");

        return false;
    }

    qCDebug(DIGIKAM_QTAV_LOG)
        << QString::fromUtf8("PulseAudio %1, protocol: %2, server protocol: %3")
            .arg(QString::fromLatin1(pa_get_library_version()))
            .arg(pa_context_get_protocol_version(ctx))
            .arg(pa_context_get_server_protocol_version(ctx));

    // TODO: host property

    pa_context_connect(ctx, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
    pa_context_set_state_callback(ctx, AudioOutputPulse::contextStateCallback, this);

    while (true)
    {
        const pa_context_state_t st = pa_context_get_state(ctx);

        if (st == PA_CONTEXT_READY)
            break;

        if (!PA_CONTEXT_IS_GOOD(st))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("PulseAudio context init failed");

            return false;
        }

        pa_threaded_mainloop_wait(loop);
    }

    pa_context_set_subscribe_callback(ctx, AudioOutputPulse::contextSubscribeCallback, this);
    pa_context_subscribe(ctx, pa_subscription_mask_t(
                                 PA_SUBSCRIPTION_MASK_CARD |
                                 PA_SUBSCRIPTION_MASK_SINK |
                                 PA_SUBSCRIPTION_MASK_SINK_INPUT),
                         nullptr, nullptr);
    // pa_sample_spec
    // setup format

    pa_format_info* const fi = pa_format_info_new();
    fi->encoding             = PA_ENCODING_PCM;
    pa_format_info_set_sample_format(fi, sampleFormatToPulse(format.sampleFormat()));
    pa_format_info_set_channels(fi, format.channels());
    pa_format_info_set_rate(fi, format.sampleRate());

    // pa_format_info_set_channel_map(fi, nullptr); // TODO

    if (!pa_format_info_valid(fi))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("PulseAudio: invalid format");

        return false;
    }

    pa_proplist* const pl = pa_proplist_new();

    if (pl)
    {
        pa_proplist_sets(pl, PA_PROP_MEDIA_ROLE, "video");
        pa_proplist_sets(pl, PA_PROP_MEDIA_ICON_NAME,
                         qApp->applicationName()
                            .append(QLatin1String(" (QtAV)")).toUtf8().constData());
    }

    stream = pa_stream_new_extended(ctx, "audio stream", &fi, 1, pl);

    if (!stream)
    {
        pa_format_info_free(fi);
        pa_proplist_free(pl);

        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("PulseAudio: failed to create a stream");

        return false;
    }

    pa_format_info_free(fi);
    pa_proplist_free(pl);
    pa_stream_set_write_callback(stream, AudioOutputPulse::writeCallback, this);
    pa_stream_set_state_callback(stream, AudioOutputPulse::stateCallback, this);
    pa_stream_set_latency_update_callback(stream, AudioOutputPulse::latencyUpdateCallback, this);

    pa_buffer_attr ba;
    ba.maxlength = PA_STREAM_ADJUST_LATENCY; // -1; // buffer_size*buffer_count; // max buffer size on the server
    ba.tlength   = PA_STREAM_ADJUST_LATENCY; // (uint32_t)-1; // ?
    ba.prebuf    = 1;                        // (uint32_t)-1; // play as soon as possible
    ba.minreq    = (uint32_t)-1;
/*
    ba.fragsize  = (uint32_t)-1;             // latency
*/
    // PA_STREAM_NOT_MONOTONIC?

    pa_stream_flags_t flags = pa_stream_flags_t(PA_STREAM_NOT_MONOTONIC      |
                                                PA_STREAM_INTERPOLATE_TIMING |
                                                PA_STREAM_AUTO_TIMING_UPDATE);

    if (pa_stream_connect_playback(stream, nullptr /*sink*/, &ba, flags, nullptr, nullptr) < 0)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("PulseAudio failed: pa_stream_connect_playback");

        return false;
    }

    while (true)
    {
        const pa_stream_state_t st = pa_stream_get_state(stream);

        if (st == PA_STREAM_READY)
            break;

        if (!PA_STREAM_IS_GOOD(st))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("PulseAudio stream init failed");

            return false;
        }

        pa_threaded_mainloop_wait(loop);
    }

    if (pa_stream_is_suspended(stream))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("PulseAudio stream is suspende");

        return false;
    }

    return true;
}

AudioOutputPulse::AudioOutputPulse(QObject* const parent)
    : AudioOutputBackend(AudioOutput::DeviceFeatures() |
                         AudioOutput::SetVolume        |
                         AudioOutput::SetMute          |
                         AudioOutput::SetSampleRate, parent),
      loop              (nullptr),
      ctx               (nullptr),
      stream            (nullptr),
      writable_size     (0)
{
/*
    setDeviceFeatures(DeviceFeatures()|SetVolume|SetMute);
*/
}

bool AudioOutputPulse::isSampleFormatSupported(AudioFormat::SampleFormat spformat) const
{
    for (int i = 0 ; format_map[i].spformat != AudioFormat::SampleFormat_Unknown ; ++i)
    {
        if (format_map[i].spformat == spformat)
            return true;
    }

    return false;
}

bool AudioOutputPulse::open()
{
    if (!init(format))
    {
        if (ctx)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("%s", pa_strerror(pa_context_errno(ctx)));
        }

        close();

        return false;
    }

    return true;
}

bool AudioOutputPulse::close()
{
    if (stream)
    {
        ScopedPALocker palock(loop);

        Q_UNUSED(palock);

        PA_ENSURE_TRUE(waitPAOperation(pa_stream_drain(stream, AudioOutputPulse::successCallback, this)), false);
    }

    if (loop)
    {
        pa_threaded_mainloop_stop(loop);
    }

    if (stream)
    {
        pa_stream_disconnect(stream);
        pa_stream_unref(stream);
        stream = nullptr;
    }

    if (ctx)
    {
        pa_context_disconnect(ctx);
        pa_context_unref(ctx);
        ctx = nullptr;
    }

    if (loop)
    {
        pa_threaded_mainloop_free(loop);
        loop = nullptr;
    }

    return true;
}

AudioOutputBackend::BufferControl AudioOutputPulse::bufferControl() const
{
    return BytesCallback;
}

int AudioOutputPulse::getWritableBytes()
{
    // return writable_size;

    if (!loop || !stream)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("pulseaudio is not open");

        return 0;
    }

    ScopedPALocker palock(loop);

    Q_UNUSED(palock);

    return pa_stream_writable_size(stream);
}

bool AudioOutputPulse::write(const QByteArray& data)
{
    ScopedPALocker palock(loop);

    Q_UNUSED(palock);

    PA_ENSURE_TRUE(pa_stream_write(stream, data.constData(), data.size(), nullptr, 0LL, PA_SEEK_RELATIVE) >= 0, false);

    writable_size -= data.size();

    return true;
}

bool AudioOutputPulse::play()
{
    return true;
}

bool AudioOutputPulse::setVolume(qreal value)
{
    ScopedPALocker palock(loop);

    Q_UNUSED(palock);

    uint32_t stream_idx = pa_stream_get_index(stream);
    struct pa_cvolume vol; // TODO: per-channel volume
    pa_cvolume_reset(&vol, format.channels());
    pa_cvolume_set(&vol, format.channels(), pa_volume_t(value*qreal(PA_VOLUME_NORM)));
    pa_operation* o     = nullptr;
    PA_ENSURE_TRUE((o = pa_context_set_sink_input_volume(ctx, stream_idx, &vol, nullptr, nullptr)) != nullptr, false);
    pa_operation_unref(o);

    return true;
}

qreal AudioOutputPulse::getVolume() const
{
    ScopedPALocker palock(loop);

    Q_UNUSED(palock);

    uint32_t stream_idx = pa_stream_get_index(stream);
    PA_ENSURE_TRUE(waitPAOperation(pa_context_get_sink_input_info(ctx,
                                                                  stream_idx,
                                                                  AudioOutputPulse::sinkInfoCallback,
                                                                  (void*)this)), 0.0);

    return (qreal)pa_cvolume_avg(&info.volume) / qreal(PA_VOLUME_NORM);
}

bool AudioOutputPulse::setMute(bool value)
{
    ScopedPALocker palock(loop);

    Q_UNUSED(palock);

    uint32_t stream_idx = pa_stream_get_index(stream);
    pa_operation* o     = nullptr;
    PA_ENSURE_TRUE((o = pa_context_set_sink_input_mute(ctx, stream_idx, value, nullptr, nullptr)) != nullptr, false);
    pa_operation_unref(o);

    return true;
}

} // namespace QtAV

#include "AudioOutputPulse.moc"
