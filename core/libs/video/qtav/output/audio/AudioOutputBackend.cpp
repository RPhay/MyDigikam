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

// Local includes

#include "QtAV_factory.h"
#include "digikam_debug.h"

namespace QtAV
{

QStringList AudioOutputBackend::defaultPriority()
{
    static const QStringList sBackends = QStringList()

#ifdef Q_OS_MAC

            << QLatin1String("AudioToolbox")

#endif

#if QTAV_HAVE(XAUDIO2)

            << QLatin1String("XAudio2")

#endif

#if QTAV_HAVE(OPENSL)

            << QLatin1String("OpenSL")

#endif

#if QTAV_HAVE(OPENAL)

            << QLatin1String("OpenAL")

#endif

#if QTAV_HAVE(PORTAUDIO)

            << QLatin1String("PortAudio")

#endif

#if QTAV_HAVE(PULSEAUDIO)&& !defined(Q_OS_MAC)

            << QLatin1String("Pulse")

#endif

#if QTAV_HAVE(DSOUND)

            << QLatin1String("DirectSound")

#endif

    ;

    return sBackends;
}

AudioOutputBackend::AudioOutputBackend(AudioOutput::DeviceFeatures f, QObject* const parent)
    : QObject     (parent),
      audio       (nullptr),
      available   (true),
      buffer_size (0),
      buffer_count(0),
      m_features  (f)
{
}

void AudioOutputBackend::onCallback()
{
    if (!audio)
        return;

    audio->onCallback();
}

FACTORY_DEFINE(AudioOutputBackend)

void AudioOutput_RegisterAll()
{
    static bool initialized = false;

    if (initialized)
        return;

    initialized = true;

    // check whether ids are registered automatically

    if (!AudioOutputBackendFactory::Instance().registeredIds().empty())
        return;

    extern bool RegisterAudioOutputBackendNull_Man();

    RegisterAudioOutputBackendNull_Man();

#ifdef Q_OS_DARWIN

    extern bool RegisterAudioOutputBackendAudioToolbox_Man();
    RegisterAudioOutputBackendAudioToolbox_Man();

#endif

#if QTAV_HAVE(OPENSL)

    extern bool RegisterAudioOutputBackendOpenSL_Man();
    RegisterAudioOutputBackendOpenSL_Man();

#endif // QTAV_HAVE(OPENSL)

#if QTAV_HAVE(XAUDIO2)

    extern bool RegisterAudioOutputBackendXAudio2_Man();
    RegisterAudioOutputBackendXAudio2_Man();

#endif

#if QTAV_HAVE(OPENAL)

    extern bool RegisterAudioOutputBackendOpenAL_Man();
    RegisterAudioOutputBackendOpenAL_Man();

#endif // QTAV_HAVE(OPENAL)

#if QTAV_HAVE(PULSEAUDIO)

    extern bool RegisterAudioOutputBackendPulse_Man();
    RegisterAudioOutputBackendPulse_Man();

#endif

#if QTAV_HAVE(PORTAUDIO)

    extern bool RegisterAudioOutputBackendPortAudio_Man();
    RegisterAudioOutputBackendPortAudio_Man();

#endif // QTAV_HAVE(PORTAUDIO)

#if QTAV_HAVE(DSOUND)

    extern bool RegisterAudioOutputBackendDSound_Man();
    RegisterAudioOutputBackendDSound_Man();

#endif

}

} // namespace QtAV
