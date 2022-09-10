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

// PortAudio includes

#include <portaudio.h>

// Local includes

#include "QtAV_mkid.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

namespace QtAV
{

static const char kName[] = "PortAudio";

class Q_DECL_HIDDEN AudioOutputPortAudio final : public AudioOutputBackend
{
    Q_OBJECT

public:

    AudioOutputPortAudio(QObject* const parent = nullptr);
    ~AudioOutputPortAudio();

    QString name() const                        final
    {
        return QString::fromLatin1(kName);
    }

    bool open()                                 final;
    bool close()                                final;
    virtual BufferControl bufferControl() const final;
    virtual bool write(const QByteArray& data)  final;

    virtual bool play()                         final
    {
        return true;
    }

private:

    bool                initialized      = false;
    PaStreamParameters* outputParameters = nullptr;
    PaStream*           stream           = nullptr;
    double              outputLatency    = 0.0;
};

typedef AudioOutputPortAudio AudioOutputBackendPortAudio;

static const AudioOutputBackendId AudioOutputBackendId_PortAudio = mkid::id32base36_5<'P', 'o', 'r', 't', 'A'>::value;

FACTORY_REGISTER(AudioOutputBackend, PortAudio, kName)

AudioOutputPortAudio::AudioOutputPortAudio(QObject* parent)
    : AudioOutputBackend(AudioOutput::NoFeature, parent),
      initialized       (false),
      outputParameters  (new PaStreamParameters),
      stream            (nullptr),
      outputLatency     (0.0)
{
    PaError err = paNoError;

    if ((err = Pa_Initialize()) != paNoError)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Error when init portaudio: %s",
                Pa_GetErrorText(err));

        return;
    }

    initialized = true;

    int numDevices = Pa_GetDeviceCount();

    for (int i = 0 ; i < numDevices ; ++i)
    {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);

        if (deviceInfo)
        {
            const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            QString pname                    = QString::fromUtf8(hostApiInfo->name) + QLatin1String(": ") + QString::fromLocal8Bit(deviceInfo->name);

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("audio device %d: %s", i, pname.toUtf8().constData());

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("max in/out channels: %d/%d",
                    deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
        }
    }

    memset(outputParameters, 0, sizeof(PaStreamParameters));
    outputParameters->device = Pa_GetDefaultOutputDevice();

    if (outputParameters->device == paNoDevice)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("PortAudio get device error!");

        return;
    }

    const PaDeviceInfo* deviceInfo              = Pa_GetDeviceInfo(outputParameters->device);

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("DEFAULT max in/out channels: %d/%d",
            deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("audio device: %s",
            QString::fromLocal8Bit(Pa_GetDeviceInfo(outputParameters->device)->name).toUtf8().constData());

    outputParameters->hostApiSpecificStreamInfo = nullptr;
    outputParameters->suggestedLatency          = Pa_GetDeviceInfo(outputParameters->device)->defaultHighOutputLatency;
}

AudioOutputPortAudio::~AudioOutputPortAudio()
{
    if (outputParameters)
    {
        delete outputParameters;
        outputParameters = nullptr;
    }
}

AudioOutputBackend::BufferControl AudioOutputPortAudio::bufferControl() const
{
    return Blocking;
}

bool AudioOutputPortAudio::write(const QByteArray& data)
{
    if (Pa_IsStreamStopped(stream))
        Pa_StartStream(stream);

    PaError err = Pa_WriteStream(stream, data.constData(), data.size() / format.channels() / format.bytesPerSample());

    if (err == paUnanticipatedHostError)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Write portaudio stream error: %s",
                Pa_GetErrorText(err));

        return false;
    }

    return true;
}

// TODO: what about planar, int8, int24 etc that FFmpeg or Pa not support?

static int toPaSampleFormat(AudioFormat::SampleFormat format)
{
    switch (format)
    {
        case AudioFormat::SampleFormat_Unsigned8:
        {
            return paUInt8;
        }

        case AudioFormat::SampleFormat_Signed16:
        {
            return paInt16;
        }

        case AudioFormat::SampleFormat_Signed32:
        {
            return paInt32;
        }

        case AudioFormat::SampleFormat_Float:
        {
            return paFloat32;
        }

        default:
        {
            return paCustomFormat;
        }
    }
}

// TODO: call open after audio format changed?

bool AudioOutputPortAudio::open()
{
    outputParameters->sampleFormat = toPaSampleFormat(format.sampleFormat());
    outputParameters->channelCount = format.channels();
    PaError err                    = Pa_OpenStream(&stream, nullptr, outputParameters, format.sampleRate(),
                                                   0, paNoFlag, nullptr, nullptr);

    if (err != paNoError)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Open portaudio stream error: %s",
                Pa_GetErrorText(err));

        return false;
    }

    outputLatency                  = Pa_GetStreamInfo(stream)->outputLatency;

    return true;
}

bool AudioOutputPortAudio::close()
{
    if (!stream)
    {
        return true;
    }

    PaError err = Pa_StopStream(stream); // may be already stopped: paStreamIsStopped

    if (err != paNoError)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Stop portaudio stream error: %s",
                Pa_GetErrorText(err));
/*
        return err == paStreamIsStopped;
*/
    }

    err = Pa_CloseStream(stream);

    if (err != paNoError)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Close portaudio stream error: %s",
                Pa_GetErrorText(err));

        return false;
    }

    stream = nullptr;

    if (initialized)
        Pa_Terminate(); // Do NOT call this if init failed. See document

    return true;
}

} // namespace QtAV

#include "AudioOutputPortAudio.moc"
