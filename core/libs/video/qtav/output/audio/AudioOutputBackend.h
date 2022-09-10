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

#ifndef QTAV_AUDIO_OUTPUT_BACKEND_H
#define QTAV_AUDIO_OUTPUT_BACKEND_H

// Qt includes

#include <QObject>

// Local includes

#include "AudioFormat.h"
#include "AudioOutput.h"

namespace QtAV
{

typedef int AudioOutputBackendId;

class DIGIKAM_EXPORT AudioOutputBackend : public QObject
{
    Q_OBJECT

public:

    AudioOutput*    audio;
    bool            available;         ///< default is true. set to false when failed to create backend
    int             buffer_size;
    int             buffer_count;
    AudioFormat     format;

    static QStringList defaultPriority();

    /*!
     * \brief AudioOutputBackend
     * Specify supported features by the backend. Use this for new backends.
     */
    AudioOutputBackend(AudioOutput::DeviceFeatures f, QObject* const parent);

    virtual ~AudioOutputBackend() {}
    virtual QString name() const                = 0;
    virtual bool open()                         = 0;
    virtual bool close()                        = 0;
    virtual bool write(const QByteArray& data)  = 0; // MUST
    virtual bool play()                         = 0; // MUST
    virtual bool flush() { return false;}
    virtual bool clear() { return false;}

    virtual bool isSupported(const AudioFormat& format) const
    {
        return (isSupported(format.sampleFormat()) && isSupported(format.channelLayout()));
    }

    // FIXME: workaround. planar convertion crash now!

    virtual bool isSupported(AudioFormat::SampleFormat f) const
    {
        return !IsPlanar(f);
    }

    // 5, 6, 7 channels may not play

    virtual bool isSupported(AudioFormat::ChannelLayout cl) const
    {
        return (int(cl) < int(AudioFormat::ChannelLayout_Unsupported));
    }

    /*!
     * \brief The BufferControl enum
     * Used to adapt to different audio playback backend.
     * Usually you don't need this in application level development.
     */
    enum BufferControl
    {
        User            = 0,      ///< You have to reimplement waitForNextBuffer()
        Blocking        = 1,
        BytesCallback   = 1 << 1,
        CountCallback   = 1 << 2,
        PlayedCount     = 1 << 3, ///< number of buffers played since last buffer dequeued
        PlayedBytes     = 1 << 4,
        OffsetIndex     = 1 << 5, ///< current playing offset
        OffsetBytes     = 1 << 6, ///< current playing offset by bytes
        WritableBytes   = 1 << 7,
    };

    virtual BufferControl bufferControl() const = 0;

    // called by callback with Callback control

    virtual void onCallback();
    virtual void acquireNextBuffer() {}

    // default return -1. means not the control

    virtual int getPlayedCount()   { return -1; }    // PlayedCount

    /*!
     * \brief getPlayedBytes
     * reimplement this if bufferControl() is PlayedBytes.
     * \return the bytes played since last dequeue the buffer queue
     */
    virtual int getPlayedBytes()   { return -1; }    // PlayedBytes
    virtual int getOffset()        { return -1; }    // OffsetIndex
    virtual int getOffsetByBytes() { return -1; }    // OffsetBytes
    virtual int getWritableBytes() { return -1; }    // WritableBytes

    // not virtual. called in ctor

    AudioOutput::DeviceFeatures supportedFeatures()
    {
        return m_features;
    }

    /*!
     * \brief setVolume
     * Set volume by backend api. If backend can not set the
     * given volume, or SetVolume feature is not set,
     * software implemention will be used.
     * \param value >=0
     * \return true if success
     */
    virtual bool setVolume(qreal value)
    {
        Q_UNUSED(value);

        return false;
    }

    virtual qreal getVolume() const
    {
        return 1.0;
    }

    virtual bool setMute(bool value = true)
    {
        Q_UNUSED(value);

        return false;
    }

    virtual bool getMute() const
    {
        return false;
    }

Q_SIGNALS:

    /*
     * \brief reportVolume
     * Volume can be changed by per-app volume control from system outside this library.
     * Useful for synchronizing ui to system.
     * Volume control from QtAV may invoke it too. And it may be invoked even if volume is not changed.
     * If volume changed, signal volumeChanged() will be emitted and volume() will be updated.
     * Only supported by some backends, e.g. pulseaudio
     */
    void volumeReported(qreal value);
    void muteReported(bool value);

public:

    template<class C> static bool Register(AudioOutputBackendId id, const char* name)
    {
        return Register(id, create<C>, name);
    }

    static AudioOutputBackend* create(AudioOutputBackendId id);
    static AudioOutputBackend* create(const char* name);

    /*!
     * \brief next
     * \param id nullptr to get the first id address
     * \return address of id or nullptr if not found/end
     */
    static AudioOutputBackendId* next(AudioOutputBackendId* id = nullptr);
    static const char* name(AudioOutputBackendId id);
    static AudioOutputBackendId id(const char* name);

private:

    template<class C> static AudioOutputBackend* create()
    {
        return new C();
    }

    typedef AudioOutputBackend* (*AudioOutputBackendCreator)();

    static bool Register(AudioOutputBackendId id, AudioOutputBackendCreator, const char* name);

private:

    AudioOutput::DeviceFeatures m_features;

    Q_DISABLE_COPY(AudioOutputBackend)
};

} // namespace QtAV

#endif // QTAV_AUDIO_OUTPUT_BACKEND_H
