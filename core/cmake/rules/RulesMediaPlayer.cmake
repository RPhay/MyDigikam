#
# SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if(ENABLE_MEDIAPLAYER)

    # NOTE: For Qt5, QtMultimedia is used to render video on widget.
    # NOTE: For Qt6, QtMultimedia based on FFMpeg is only supported with version > 6.5.

    find_package(Qt${QT_VERSION_MAJOR}
                 OPTIONAL_COMPONENTS
                 Multimedia
                 MultimediaWidgets
    )

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Multimedia,INTERFACE_INCLUDE_DIRECTORIES>
                        $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::MultimediaWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    if(Qt6_FOUND)

        include_directories(${CMAKE_SOURCE_DIR}/core/libs/video/player/qtmm)


        set(ENABLE_QTMULTIMEDIA ON)
        message(STATUS "MediaPlayer type:     Qt6::Multimedia")

    else()

        set(ENABLE_QTMULTIMEDIA OFF)

        if(NOT ${FFMPEG_FOUND})

            set(ENABLE_MEDIAPLAYER OFF)
            message(STATUS "MediaPlayer type:     None")

        else()

            message(STATUS "--------------------------------------------------")
            message(STATUS "QtAV dependencies checks:")
            message(STATUS "")

            find_package(Portaudio  QUIET)
            find_package(PulseAudio QUIET)
            find_package(VAAPI      QUIET)
            find_package(OpenSLES   QUIET)

            if(WIN32)

                set(DirectX_FIND_REQUIRED_D3D11 TRUE)
                find_package(DirectX)
                find_package(Direct2D)
                find_package(GDIPLUS)

            endif()

            include(MacroSSE)
            CheckSSESupport()

            MACRO_BOOL_TO_01(PORTAUDIO_FOUND       HAVE_LIBPORTAUDIO)
            MACRO_BOOL_TO_01(PULSEAUDIO_FOUND      HAVE_LIBPULSEAUDIO)
            MACRO_BOOL_TO_01(VAAPI_FOUND           HAVE_LIBVAAPI)
            MACRO_BOOL_TO_01(DirectX_D3D11_FOUND   HAVE_LIBD3D11)
            MACRO_BOOL_TO_01(DirectX_DSound_FOUND  HAVE_LIBDIRECTSOUND)
            MACRO_BOOL_TO_01(DirectX_XAudio2_FOUND HAVE_LIBXAUDIO2)
            MACRO_BOOL_TO_01(GDIPLUS_FOUND         HAVE_LIBGDIPLUS)
            MACRO_BOOL_TO_01(Direct2D_FOUND        HAVE_LIBDIRECT2D)
            MACRO_BOOL_TO_01(OPENSLES_FOUND        HAVE_LIBOPENSLES)

            # --- Reports and Libraries -----------------------------------------------------------

            if(SSE4_1_FOUND)

                message(STATUS "MediaPlayer will be compiled with SSE4.1 support     : yes")

                if (NOT MSVC)

                    set(MEDIAPLAYER_FLAGS ${MEDIAPLAYER_FLAGS} -msse4.1)

                endif()

            endif()

            if(SSE2_FOUND)

                message(STATUS "MediaPlayer will be compiled with SSE2 support       : yes")

                if (NOT MSVC)

                    set(MEDIAPLAYER_FLAGS ${MEDIAPLAYER_FLAGS} -msse2)

                endif()

            endif()

            if(PORTAUDIO_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${PORTAUDIO_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with PortAudio support  : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with PortAudio support  : no")

            endif()

            if(PULSEAUDIO_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${PULSEAUDIO_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with PulseAudio support : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with PulseAudio support : no")

            endif()

            if(OPENSLES_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENSLES_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with OpenSLES support   : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with OpenSLES support   : no")

            endif()

            if(VAAPI_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${VAAPI_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with VaAPI support      : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with VaAPI support      : no")

            endif()

            if(X11_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${X11_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with X11 support        : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with X11 support        : no")

            endif()

            if(X11_Xv_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${X11_Xv_LIB})
                message(STATUS "MediaPlayer will be compiled with XV support         : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with XV support         : no")

            endif()

            if(OPENGL_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENGL_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGL)

                if(Qt6_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

                endif()

                message(STATUS "MediaPlayer will be compiled with OpenGL support     : yes")

                if(OpenGL_EGL_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENGL_egl_LIBRARY})
                    message(STATUS "MediaPlayer will be compiled with OpenGL-EGL support : yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with OpenGL-EGL support : no")

                endif()

            else()

                message(STATUS "MediaPlayer will be compiled with OpenGL support     : no")
                message(STATUS "MediaPlayer will be compiled with OpenGL-EGL support : no")

            endif()

            # --- Windows config ---

            if(GDIPLUS_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${GDIPLUS_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with GDI+ support       : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with GDI+ support       : no")

            endif()

            if(DIRECTX_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${DIRECT3D_LIBRARIES})

                if(DirectX_D3D11_FOUND)

                    message(STATUS "MediaPlayer will be compiled with Direct3D11 support : yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with Direct3D11 support : no")

                endif()


                if(DirectX_DSound_FOUND)

                    message(STATUS "MediaPlayer will be compiled with DirectSound support: yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with DirectSound support: no")

                endif()

                if(DirectX_XAudio2_FOUND)

                    message(STATUS "MediaPlayer will be compiled with XAudio2 support    : yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with XAudio2 support    : no")

                endif()

            else()

                message(STATUS "MediaPlayer will be compiled with Direct3D11 support : no")
                message(STATUS "MediaPlayer will be compiled with DirectSound support: no")
                message(STATUS "MediaPlayer will be compiled with XAudio2 support    : no")

            endif()

            if(Direct2D_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${Direct2D_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with Direct2D support   : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with Direct2D support   : no")

            endif()

            # --- MacOS config ---

            if(APPLE)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} "-framework CoreMedia"
                                                                   "-framework CoreVideo"
                                                                   "-framework VideoToolbox"
                                                                   "-framework AudioToolbox"
                                                                   "-framework CoreFoundation"
                                                                   "-framework VideoDecodeAcceleration"
                )

                if(AVDEVICE_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} "-framework Foundation"
                                                                       "-framework QuartzCore"
                                                                       "-framework CoreGraphics"
                                                                       "-framework AVFoundation"
                    )

                endif()

                if(AVFILTER_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} "-framework AppKit")

                endif()

            endif()

            # --- Definitions -------------------------------------------------------------------------------------

            set(MEDIAPLAYER_DEFINITIONS -D__STDC_CONSTANT_MACROS
                                        -DQTAV_HAVE_CAPI=1           # To load libass, vaapi, EGL, and more dynamically
                                        -DQTAV_HAVE_VDA=0            # Hardware acceleration video decoder removed with ffmpeg  4.0
                                        -DAUTO_REGISTER=0            # Only used in QtAVWidgets_Global.h => ???
                                        -DFF_API_OLD_MSMPEG4=0       # Used with commented code from VideoDecoderFFmpeg.cpp
                                        -DFF_API_AC_VLC=0            # Used with commented code from VideoDecoderFFmpeg.cpp
            )

            if(FFMPEG_VER5_FOUND)

                # This definition is also used outside QtAV code.

                add_definitions(-DHAVE_FFMPEG_VERSION5)

            endif()

            if(NOT SSE4_1_FOUND AND NOT SSE2_FOUND)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE4_1=0)
                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE2=0)

            elseif(SSE4_1_FOUND)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE4_1=1)
                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE2=1)

            elseif(SSE2_FOUND)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE4_1=0)
                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE2=1)

            endif()

            if(AVCODEC_FOUND AND DirectX_D3D11_FOUND)

                # DirectX VA for Windows only.  Depends of FFMpeg::libavcodec/dxva2.h

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_DXVA=1)

            else()

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_DXVA=0)

            endif()

            if(OpenGL_EGL_FOUND)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DCAPI_LINK_EGL)

            endif()

            if(NOT Qt6_FOUND)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_GL=${HAVE_LIBOPENGL})               # For QtAVWidgets with Qt < 6

            else()

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_GL=0)

            endif()

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_X11=${HAVE_LIBX11})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_XV=${HAVE_LIBXV})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_PORTAUDIO=${HAVE_LIBPORTAUDIO})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_PULSEAUDIO=${HAVE_LIBPULSEAUDIO})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_OPENSL=${HAVE_LIBOPENSLES})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_D3D11VA=${HAVE_LIBD3D11})           # DirectX 3D for MSVC only
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_XAUDIO2=${HAVE_LIBXAUDIO2})         # XAudio2 for MSVC only (replacement of DirectSound)
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_DSOUND=${HAVE_LIBDIRECTSOUND})      # DirectX Sound for MSVC only (replaced by XAudio2)
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_GDIPLUS=${HAVE_LIBGDIPLUS})         # Graphics Device Interface Plus for MSVC only (replaced by DirectX)
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_DIRECT2D=${HAVE_LIBDIRECT2D})       # DirectX 2D for MSVC only (replaced by DirectX 3D)

            # Use libavresample if libswresample is not available.
            # https://github.com/xbmc/xbmc/commit/274679d

            if(AVRESAMPLE_FOUND AND NOT SWRESAMPLE_FOUND)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SWR_AVR_MAP=1)

            else()

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SWR_AVR_MAP=0)

            endif()

            if(APPLE)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_VIDEOTOOLBOX=1)
                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_DLLAPI_CUDA=0)
                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_CUDA=0)

            else()

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_VIDEOTOOLBOX=0)
                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_DLLAPI_CUDA=0)

                if(FFMPEG_VER5_FOUND)

                    # FIXME: CUDA is currently disabled with FFMPEG version 5. Code need to be ported to new API

                    set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_CUDA=0)

                else()

                    set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_CUDA=1)

                endif()

            endif()

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVCODEC=${HAVE_LIBAVCODEC})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVDEVICE=${HAVE_LIBAVDEVICE})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVFILTER=${HAVE_LIBAVFILTER})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVUTIL=${HAVE_LIBAVUTIL})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SWSCALE=${HAVE_LIBSWSCALE})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SWRESAMPLE=${HAVE_LIBSWRESAMPLE})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVRESAMPLE=${HAVE_LIBAVRESAMPLE})
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_EGL_CAPI=${HAVE_LIBOPENGL_EGL})

            if(FFMPEG_VER5_FOUND)

                # FIXME: VAAPI is currently disabled with FFMPEG version 5. Code need to be ported to new API

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_VAAPI=0)

            else()

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_VAAPI=${HAVE_LIBVAAPI})

            endif()

            # --- Resume ---------------------------------------------------------------------------------------

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQT_AVPLAYER_MULTIMEDIA)

            message(STATUS "MediaPlayer type       : QtAVPlayer")
            message(STATUS "MediaPlayer libraries  : ${MEDIAPLAYER_LIBRARIES}")
            message(STATUS "MediaPlayer definitions: ${MEDIAPLAYER_DEFINITIONS}")
            message(STATUS "MediaPlayer flags      : ${MEDIAPLAYER_FLAGS}")

            include_directories(${CMAKE_SOURCE_DIR}/core/libs/video/QtAVPlayer)
            include_directories(${CMAKE_SOURCE_DIR}/core/libs/video/player/QtAVPlayer)

        endif()

    endif()

endif()
