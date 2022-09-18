/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText: 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================*/

#define OPENAL_CAPI_BUILD

/*
#define DEBUG_RESOLVE
#define DEBUG_LOAD                // why affects other xxx_api.cpp?
#define CAPI_IS_LAZY_RESOLVE 0
*/

#ifndef CAPI_LINK_OPENAL
#   include "QtAV_capi.h"
#endif

#include "openal_api.h" // include last to avoid covering types later

namespace openal
{

#ifdef CAPI_LINK_OPENAL

api::api()               { dll = nullptr; }
api::~api()              {                }
bool api::loaded() const { return true;   }

#else

static const char* names[] =
{
    "openal",

#ifdef CAPI_TARGET_OS_WIN

    "OpenAL32",

#endif

#ifdef __APPLE__                                          // krazy:exclude=cpp

    "/System/Library/Frameworks/OpenAL.framework/OpenAL", // iOS and macOS

#endif

    "OpenAL",                                             // blackberry
    nullptr
};

typedef ::capi::dso user_dso;

#if 1

static const int versions[] =
{
    ::capi::NoVersion,

    // the following line will be replaced by the content of config/openal/version if exists

    1,
    ::capi::EndVersion
};

CAPI_BEGIN_DLL_VER(names, versions, user_dso)

# else

CAPI_BEGIN_DLL(names, user_dso)

# endif

// CAPI_DEFINE_RESOLVER(argc, return_type, name, argv_no_name)

CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alDopplerFactor, CAPI_ARG1(ALfloat))                                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alDopplerVelocity, CAPI_ARG1(ALfloat))                                                 // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSpeedOfSound, CAPI_ARG1(ALfloat))                                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alDistanceModel, CAPI_ARG1(ALenum))                                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alEnable, CAPI_ARG1(ALenum))                                                           // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alDisable, CAPI_ARG1(ALenum))                                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALboolean, AL_APIENTRY, alIsEnabled, CAPI_ARG1(ALenum))                                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(const ALchar*, AL_APIENTRY, alGetString, CAPI_ARG1(ALenum))                                               // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetBooleanv, CAPI_ARG2(ALenum, ALboolean*))                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetIntegerv, CAPI_ARG2(ALenum, ALint*))                                              // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetFloatv, CAPI_ARG2(ALenum, ALfloat*))                                              // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetDoublev, CAPI_ARG2(ALenum, ALdouble*))                                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALboolean, AL_APIENTRY, alGetBoolean, CAPI_ARG1(ALenum))                                                  // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALint, AL_APIENTRY, alGetInteger, CAPI_ARG1(ALenum))                                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALfloat, AL_APIENTRY, alGetFloat, CAPI_ARG1(ALenum))                                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALdouble, AL_APIENTRY, alGetDouble, CAPI_ARG1(ALenum))                                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALenum, AL_APIENTRY, alGetError, CAPI_ARG0())                                                             // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALboolean, AL_APIENTRY, alIsExtensionPresent, CAPI_ARG1(const ALchar*))                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void*, AL_APIENTRY, alGetProcAddress, CAPI_ARG1(const ALchar*))                                           // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALenum, AL_APIENTRY, alGetEnumValue, CAPI_ARG1(const ALchar*))                                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alListenerf, CAPI_ARG2(ALenum, ALfloat))                                               // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alListener3f, CAPI_ARG4(ALenum, ALfloat, ALfloat, ALfloat))                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alListenerfv, CAPI_ARG2(ALenum, const ALfloat*))                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alListeneri, CAPI_ARG2(ALenum, ALint))                                                 // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alListener3i, CAPI_ARG4(ALenum, ALint, ALint, ALint))                                  // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alListeneriv, CAPI_ARG2(ALenum, const ALint*))                                         // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetListenerf, CAPI_ARG2(ALenum, ALfloat*))                                           // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetListener3f, CAPI_ARG4(ALenum, ALfloat*, ALfloat*, ALfloat*))                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetListenerfv, CAPI_ARG2(ALenum, ALfloat*))                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetListeneri, CAPI_ARG2(ALenum, ALint*))                                             // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetListener3i, CAPI_ARG4(ALenum, ALint*, ALint*, ALint*))                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetListeneriv, CAPI_ARG2(ALenum, ALint*))                                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGenSources, CAPI_ARG2(ALsizei, ALuint*))                                             // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alDeleteSources, CAPI_ARG2(ALsizei, const ALuint*))                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALboolean, AL_APIENTRY, alIsSource, CAPI_ARG1(ALuint))                                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourcef, CAPI_ARG3(ALuint, ALenum, ALfloat))                                         // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSource3f, CAPI_ARG5(ALuint, ALenum, ALfloat, ALfloat, ALfloat))                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourcefv, CAPI_ARG3(ALuint, ALenum, const ALfloat*))                                 // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourcei, CAPI_ARG3(ALuint, ALenum, ALint))                                           // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSource3i, CAPI_ARG5(ALuint, ALenum, ALint, ALint, ALint))                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourceiv, CAPI_ARG3(ALuint, ALenum, const ALint*))                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetSourcef, CAPI_ARG3(ALuint, ALenum, ALfloat*))                                     // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetSource3f, CAPI_ARG5(ALuint, ALenum, ALfloat*, ALfloat*, ALfloat*))                // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetSourcefv, CAPI_ARG3(ALuint, ALenum, ALfloat*))                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetSourcei, CAPI_ARG3(ALuint, ALenum, ALint*))                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetSource3i, CAPI_ARG5(ALuint, ALenum, ALint*, ALint*, ALint*))                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetSourceiv, CAPI_ARG3(ALuint, ALenum, ALint*))                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourcePlayv, CAPI_ARG2(ALsizei, const ALuint*))                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourceStopv, CAPI_ARG2(ALsizei, const ALuint*))                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourceRewindv, CAPI_ARG2(ALsizei, const ALuint*))                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourcePausev, CAPI_ARG2(ALsizei, const ALuint*))                                     // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourcePlay, CAPI_ARG1(ALuint))                                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourceStop, CAPI_ARG1(ALuint))                                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourceRewind, CAPI_ARG1(ALuint))                                                     // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourcePause, CAPI_ARG1(ALuint))                                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourceQueueBuffers, CAPI_ARG3(ALuint, ALsizei, const ALuint*))                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alSourceUnqueueBuffers, CAPI_ARG3(ALuint, ALsizei, ALuint*))                           // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGenBuffers, CAPI_ARG2(ALsizei, ALuint*))                                             // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alDeleteBuffers, CAPI_ARG2(ALsizei, const ALuint*))                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALboolean, AL_APIENTRY, alIsBuffer, CAPI_ARG1(ALuint))                                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alBufferData, CAPI_ARG5(ALuint, ALenum, const ALvoid*, ALsizei, ALsizei))              // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alBufferf, CAPI_ARG3(ALuint, ALenum, ALfloat))                                         // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alBuffer3f, CAPI_ARG5(ALuint, ALenum, ALfloat, ALfloat, ALfloat))                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alBufferfv, CAPI_ARG3(ALuint, ALenum, const ALfloat*))                                 // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alBufferi, CAPI_ARG3(ALuint, ALenum, ALint))                                           // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alBuffer3i, CAPI_ARG5(ALuint, ALenum, ALint, ALint, ALint))                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alBufferiv, CAPI_ARG3(ALuint, ALenum, const ALint*))                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetBufferf, CAPI_ARG3(ALuint, ALenum, ALfloat*))                                     // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetBuffer3f, CAPI_ARG5(ALuint, ALenum, ALfloat*, ALfloat*, ALfloat*))                // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetBufferfv, CAPI_ARG3(ALuint, ALenum, ALfloat*))                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetBufferi, CAPI_ARG3(ALuint, ALenum, ALint*))                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetBuffer3i, CAPI_ARG5(ALuint, ALenum, ALint*, ALint*, ALint*))                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, AL_APIENTRY, alGetBufferiv, CAPI_ARG3(ALuint, ALenum, ALint*))                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCcontext*, ALC_APIENTRY, alcCreateContext, CAPI_ARG2(ALCdevice*, const ALCint*))                        // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCboolean, ALC_APIENTRY, alcMakeContextCurrent, CAPI_ARG1(ALCcontext*))                                  // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, ALC_APIENTRY, alcProcessContext, CAPI_ARG1(ALCcontext*))                                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, ALC_APIENTRY, alcSuspendContext, CAPI_ARG1(ALCcontext*))                                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, ALC_APIENTRY, alcDestroyContext, CAPI_ARG1(ALCcontext*))                                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCcontext*, ALC_APIENTRY, alcGetCurrentContext, CAPI_ARG0())                                             // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCdevice*, ALC_APIENTRY, alcGetContextsDevice, CAPI_ARG1(ALCcontext*))                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCdevice*, ALC_APIENTRY, alcOpenDevice, CAPI_ARG1(const ALCchar*))                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCboolean, ALC_APIENTRY, alcCloseDevice, CAPI_ARG1(ALCdevice*))                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCenum, ALC_APIENTRY, alcGetError, CAPI_ARG1(ALCdevice*))                                                // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCboolean, ALC_APIENTRY, alcIsExtensionPresent, CAPI_ARG2(ALCdevice*, const ALCchar*))                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void*, ALC_APIENTRY, alcGetProcAddress, CAPI_ARG2(ALCdevice*, const ALCchar*))                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCenum, ALC_APIENTRY, alcGetEnumValue, CAPI_ARG2(ALCdevice*, const ALCchar*))                            // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(const ALCchar*, ALC_APIENTRY, alcGetString, CAPI_ARG2(ALCdevice*, ALCenum))                               // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, ALC_APIENTRY, alcGetIntegerv, CAPI_ARG4(ALCdevice*, ALCenum, ALCsizei, ALCint*))                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCdevice*, ALC_APIENTRY, alcCaptureOpenDevice, CAPI_ARG4(const ALCchar*, ALCuint, ALCenum, ALCsizei))    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(ALCboolean, ALC_APIENTRY, alcCaptureCloseDevice, CAPI_ARG1(ALCdevice*))                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, ALC_APIENTRY, alcCaptureStart, CAPI_ARG1(ALCdevice*))                                               // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, ALC_APIENTRY, alcCaptureStop, CAPI_ARG1(ALCdevice*))                                                // cppcheck-suppress thisSubtraction
CAPI_DEFINE_M_ENTRY(void, ALC_APIENTRY, alcCaptureSamples, CAPI_ARG3(ALCdevice*, ALCvoid*, ALCsizei))                         // cppcheck-suppress thisSubtraction

CAPI_END_DLL()

CAPI_DEFINE_DLL             // cppcheck-suppress[noOperatorEq,noCopyConstructor]

// CAPI_DEFINE(argc, return_type, name, argv_no_name)

CAPI_DEFINE(void, alDopplerFactor, CAPI_ARG1(ALfloat))                                                  //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alDopplerVelocity, CAPI_ARG1(ALfloat))                                                //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSpeedOfSound, CAPI_ARG1(ALfloat))                                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alDistanceModel, CAPI_ARG1(ALenum))                                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alEnable, CAPI_ARG1(ALenum))                                                          //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alDisable, CAPI_ARG1(ALenum))                                                         //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALboolean, alIsEnabled, CAPI_ARG1(ALenum))                                                  //  clazy:exclude=returning-void-expression
CAPI_DEFINE(const ALchar*, alGetString, CAPI_ARG1(ALenum))                                              //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetBooleanv, CAPI_ARG2(ALenum, ALboolean*))                                         //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetIntegerv, CAPI_ARG2(ALenum, ALint*))                                             //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetFloatv, CAPI_ARG2(ALenum, ALfloat*))                                             //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetDoublev, CAPI_ARG2(ALenum, ALdouble*))                                           //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALboolean, alGetBoolean, CAPI_ARG1(ALenum))                                                 //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALint, alGetInteger, CAPI_ARG1(ALenum))                                                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALfloat, alGetFloat, CAPI_ARG1(ALenum))                                                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALdouble, alGetDouble, CAPI_ARG1(ALenum))                                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALenum, alGetError, CAPI_ARG0())                                                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALboolean, alIsExtensionPresent, CAPI_ARG1(const ALchar*))                                  //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void*, alGetProcAddress, CAPI_ARG1(const ALchar*))                                          //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALenum, alGetEnumValue, CAPI_ARG1(const ALchar*))                                           //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alListenerf, CAPI_ARG2(ALenum, ALfloat))                                              //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alListener3f, CAPI_ARG4(ALenum, ALfloat, ALfloat, ALfloat))                           //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alListenerfv, CAPI_ARG2(ALenum, const ALfloat*))                                      //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alListeneri, CAPI_ARG2(ALenum, ALint))                                                //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alListener3i, CAPI_ARG4(ALenum, ALint, ALint, ALint))                                 //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alListeneriv, CAPI_ARG2(ALenum, const ALint*))                                        //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetListenerf, CAPI_ARG2(ALenum, ALfloat*))                                          //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetListener3f, CAPI_ARG4(ALenum, ALfloat*, ALfloat*, ALfloat*))                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetListenerfv, CAPI_ARG2(ALenum, ALfloat*))                                         //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetListeneri, CAPI_ARG2(ALenum, ALint*))                                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetListener3i, CAPI_ARG4(ALenum, ALint*, ALint*, ALint*))                           //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetListeneriv, CAPI_ARG2(ALenum, ALint*))                                           //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGenSources, CAPI_ARG2(ALsizei, ALuint*))                                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alDeleteSources, CAPI_ARG2(ALsizei, const ALuint*))                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALboolean, alIsSource, CAPI_ARG1(ALuint))                                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourcef, CAPI_ARG3(ALuint, ALenum, ALfloat))                                        //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSource3f, CAPI_ARG5(ALuint, ALenum, ALfloat, ALfloat, ALfloat))                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourcefv, CAPI_ARG3(ALuint, ALenum, const ALfloat*))                                //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourcei, CAPI_ARG3(ALuint, ALenum, ALint))                                          //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSource3i, CAPI_ARG5(ALuint, ALenum, ALint, ALint, ALint))                           //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourceiv, CAPI_ARG3(ALuint, ALenum, const ALint*))                                  //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetSourcef, CAPI_ARG3(ALuint, ALenum, ALfloat*))                                    //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetSource3f, CAPI_ARG5(ALuint, ALenum, ALfloat*, ALfloat*, ALfloat*))               //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetSourcefv, CAPI_ARG3(ALuint, ALenum, ALfloat*))                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetSourcei, CAPI_ARG3(ALuint, ALenum, ALint*))                                      //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetSource3i, CAPI_ARG5(ALuint, ALenum, ALint*, ALint*, ALint*))                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetSourceiv, CAPI_ARG3(ALuint, ALenum, ALint*))                                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourcePlayv, CAPI_ARG2(ALsizei, const ALuint*))                                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourceStopv, CAPI_ARG2(ALsizei, const ALuint*))                                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourceRewindv, CAPI_ARG2(ALsizei, const ALuint*))                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourcePausev, CAPI_ARG2(ALsizei, const ALuint*))                                    //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourcePlay, CAPI_ARG1(ALuint))                                                      //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourceStop, CAPI_ARG1(ALuint))                                                      //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourceRewind, CAPI_ARG1(ALuint))                                                    //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourcePause, CAPI_ARG1(ALuint))                                                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourceQueueBuffers, CAPI_ARG3(ALuint, ALsizei, const ALuint*))                      //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alSourceUnqueueBuffers, CAPI_ARG3(ALuint, ALsizei, ALuint*))                          //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGenBuffers, CAPI_ARG2(ALsizei, ALuint*))                                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alDeleteBuffers, CAPI_ARG2(ALsizei, const ALuint*))                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALboolean, alIsBuffer, CAPI_ARG1(ALuint))                                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alBufferData, CAPI_ARG5(ALuint, ALenum, const ALvoid*, ALsizei, ALsizei))             //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alBufferf, CAPI_ARG3(ALuint, ALenum, ALfloat))                                        //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alBuffer3f, CAPI_ARG5(ALuint, ALenum, ALfloat, ALfloat, ALfloat))                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alBufferfv, CAPI_ARG3(ALuint, ALenum, const ALfloat*))                                //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alBufferi, CAPI_ARG3(ALuint, ALenum, ALint))                                          //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alBuffer3i, CAPI_ARG5(ALuint, ALenum, ALint, ALint, ALint))                           //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alBufferiv, CAPI_ARG3(ALuint, ALenum, const ALint*))                                  //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetBufferf, CAPI_ARG3(ALuint, ALenum, ALfloat*))                                    //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetBuffer3f, CAPI_ARG5(ALuint, ALenum, ALfloat*, ALfloat*, ALfloat*))               //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetBufferfv, CAPI_ARG3(ALuint, ALenum, ALfloat*))                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetBufferi, CAPI_ARG3(ALuint, ALenum, ALint*))                                      //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetBuffer3i, CAPI_ARG5(ALuint, ALenum, ALint*, ALint*, ALint*))                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alGetBufferiv, CAPI_ARG3(ALuint, ALenum, ALint*))                                     //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCcontext*, alcCreateContext, CAPI_ARG2(ALCdevice*, const ALCint*))                        //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCboolean, alcMakeContextCurrent, CAPI_ARG1(ALCcontext*))                                  //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alcProcessContext, CAPI_ARG1(ALCcontext*))                                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alcSuspendContext, CAPI_ARG1(ALCcontext*))                                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alcDestroyContext, CAPI_ARG1(ALCcontext*))                                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCcontext*, alcGetCurrentContext, CAPI_ARG0())                                             //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCdevice*, alcGetContextsDevice, CAPI_ARG1(ALCcontext*))                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCdevice*, alcOpenDevice, CAPI_ARG1(const ALCchar*))                                       //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCboolean, alcCloseDevice, CAPI_ARG1(ALCdevice*))                                          //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCenum, alcGetError, CAPI_ARG1(ALCdevice*))                                                //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCboolean, alcIsExtensionPresent, CAPI_ARG2(ALCdevice*, const ALCchar*))                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void*, alcGetProcAddress, CAPI_ARG2(ALCdevice*, const ALCchar*))                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCenum, alcGetEnumValue, CAPI_ARG2(ALCdevice*, const ALCchar*))                            //  clazy:exclude=returning-void-expression
CAPI_DEFINE(const ALCchar*, alcGetString, CAPI_ARG2(ALCdevice*, ALCenum))                               //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alcGetIntegerv, CAPI_ARG4(ALCdevice*, ALCenum, ALCsizei, ALCint*))                    //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCdevice*, alcCaptureOpenDevice, CAPI_ARG4(const ALCchar*, ALCuint, ALCenum, ALCsizei))    //  clazy:exclude=returning-void-expression
CAPI_DEFINE(ALCboolean, alcCaptureCloseDevice, CAPI_ARG1(ALCdevice*))                                   //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alcCaptureStart, CAPI_ARG1(ALCdevice*))                                               //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alcCaptureStop, CAPI_ARG1(ALCdevice*))                                                //  clazy:exclude=returning-void-expression
CAPI_DEFINE(void, alcCaptureSamples, CAPI_ARG3(ALCdevice*, ALCvoid*, ALCsizei))                         //  clazy:exclude=returning-void-expression

#endif

} // namespace openal
