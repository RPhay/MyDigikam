#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if(ENABLE_MEDIAPLAYER)

    message(STATUS "")
    message(STATUS "--------------------------------------------------")
    message(STATUS "digiKam MediaPlayer dependencies checks:")
    message(STATUS "")

    find_package(FFmpeg REQUIRED COMPONENTS AVCODEC
                                            AVDEVICE
                                            AVFILTER
                                            AVFORMAT
                                            AVUTIL
                                            SWSCALE
                                            SWRESAMPLE
    )

    find_package(FFmpeg OPTIONAL_COMPONENTS AVRESAMPLE)     # removed with ffmpeg 5


    find_package(ASS        QUIET)
    find_package(OpenAL     QUIET)
    find_package(Portaudio  QUIET)
    find_package(PulseAudio QUIET)
    find_package(VAAPI      QUIET)
#    find_package(uchardet   QUIET)
    find_package(OpenSLES   QUIET)

    if(WIN32)

        set(DirectX_FIND_REQUIRED_D3D11 TRUE)
        find_package(DirectX)
        find_package(Direct2D)
        find_package(GDIPLUS)

    endif()

    include(MacroSSE)
    CheckSSESupport()

    message(STATUS "FFMpeg AVCodec    (required) : ${AVCODEC_FOUND} (${AVCODEC_VERSION})")
    message(STATUS "FFMpeg AVDevice   (required) : ${AVDEVICE_FOUND} (${AVDEVICE_VERSION})")
    message(STATUS "FFMpeg AVFilter   (required) : ${AVFILTER_FOUND} (${AVFILTER_VERSION})")
    message(STATUS "FFMpeg AVFormat   (required) : ${AVFORMAT_FOUND} (${AVFORMAT_VERSION})")
    message(STATUS "FFMpeg AVUtil     (required) : ${AVUTIL_FOUND} (${AVUTIL_VERSION})")
    message(STATUS "FFMpeg SWScale    (required) : ${SWSCALE_FOUND} (${SWSCALE_VERSION})")
    message(STATUS "FFMpeg SWResample (required) : ${SWRESAMPLE_FOUND} (${SWRESAMPLE_VERSION})")
    message(STATUS "FFMpeg AVResample (optional) : ${AVRESAMPLE_FOUND} (${AVRESAMPLE_VERSION})")

    if(${AVCODEC_FOUND}     AND
       ${AVDEVICE_FOUND}    AND
       ${AVFILTER_FOUND}    AND
       ${AVFORMAT_FOUND}    AND
       ${AVUTIL_FOUND}      AND
       ${SWSCALE_FOUND}     AND
       ${SWRESAMPLE_FOUND}
      )

        include_directories(${FFMPEG_INCLUDE_DIRS})
        set(FFMPEG_FOUND ON)
        message(STATUS "MediaPlayer support is enabled                       : yes")

    else()

        set(ENABLE_MEDIAPLAYER OFF)
        set(FFMPEG_FOUND OFF)
        message(STATUS "MediaPlayer support is enabled                       : no")

    endif()

    if (${FFMPEG_FOUND})

        # Check if FFMPEG 5 API is available

        if (AVCODEC_VERSION)

            string(REPLACE "." ";" VERSION_LIST ${AVCODEC_VERSION})
            list(GET VERSION_LIST 0 AVCODEC_VERSION_MAJOR)
            list(GET VERSION_LIST 1 AVCODEC_VERSION_MINOR)
            list(GET VERSION_LIST 2 AVCODEC_VERSION_PATCH)


            if (${AVCODEC_VERSION_MAJOR} GREATER_EQUAL 59)

                set(FFMPEG_VER5_FOUND 1)
                message(WARNING "FFMPEG API version 5 detected: support is under developement and not yet finalized! "
                                "Please install lasted FFMPEG API version 4 LTS instead for a better video experience... "
                                "More info at https://ffmpeg.org/download.html")

            endif()

        endif()

        if (FFMPEG_VER5_FOUND)
            message(STATUS "MediaPlayer will use FFMpeg 5 API                    : yes")
        else()
            message(STATUS "MediaPlayer will use FFMpeg 5 API                    : no")
        endif()

        MACRO_BOOL_TO_01(AVCODEC_FOUND         HAVE_LIBAVCODEC)
        MACRO_BOOL_TO_01(AVDEVICE_FOUND        HAVE_LIBAVDEVICE)
        MACRO_BOOL_TO_01(AVFILTER_FOUND        HAVE_LIBAVFILTER)
        MACRO_BOOL_TO_01(AVFORMAT_FOUND        HAVE_LIBAVFORMAT)
        MACRO_BOOL_TO_01(AVUTIL_FOUND          HAVE_LIBAVUTIL)
        MACRO_BOOL_TO_01(SWSCALE_FOUND         HAVE_LIBSWSCALE)
        MACRO_BOOL_TO_01(SWRESAMPLE_FOUND      HAVE_LIBSWRESAMPLE)
        MACRO_BOOL_TO_01(AVRESAMPLE_FOUND      HAVE_LIBAVRESAMPLE)
        MACRO_BOOL_TO_01(ASS_FOUND             HAVE_LIBASS)
        MACRO_BOOL_TO_01(uchardet_FOUND        HAVE_LIBUCHARDET)
        MACRO_BOOL_TO_01(OPENAL_FOUND          HAVE_LIBOPENAL)
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

                set(MEDIAPLAYER_FLAGS -msse4.1)

            endif()

        elseif(SSE2_FOUND)

            message(STATUS "MediaPlayer will be compiled with SSE2 support       : yes")

            if (NOT MSVC)

                set(MEDIAPLAYER_FLAGS -msse2)

            endif()

        endif()

        if(NOT AVRESAMPLE_FOUND)

            set(AVRESAMPLE_LIBRARIES "")

        endif()

        set(MEDIAPLAYER_LIBRARIES ${AVCODEC_LIBRARIES}
                                  ${AVDEVICE_LIBRARIES}
                                  ${AVFILTER_LIBRARIES}
                                  ${AVFORMAT_LIBRARIES}
                                  ${AVUTIL_LIBRARIES}
                                  ${SWSCALE_LIBRARIES}
                                  ${SWRESAMPLE_LIBRARIES}
                                  ${AVRESAMPLE_LIBRARIES}   # optional with FFMPEG 4 and removed with FFMPEG 5
                                  ${CMAKE_DL_LIBS}
        )

        if(ASS_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${ASS_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with LibASS support     : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with LibASS support     : no")

        endif()

        if(uchardet_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} uchardet)
            message(STATUS "MediaPlayer will be compiled with UCharDet support   : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with UCharDet support   : no")

        endif()

        if(OPENAL_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENAL_LIBRARY})
            message(STATUS "MediaPlayer will be compiled with OpenAL support     : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with OpenAL support     : no")

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

            if(OPENAL_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} "-framework OpenAL")

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

        if(SSE4_1_FOUND)

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE4_1=1)
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE2=0)

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

        if(ASS_FOUND)

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DCAPI_LINK_ASS)

        endif()

        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_LIBASS=${HAVE_LIBASS})

        if (uchardet_FOUND)

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DLINK_UCHARDET)

        endif()

        if(OPENAL_FOUND)

            if(APPLE)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DHEADER_OPENAL_PREFIX)

            endif()

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
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_OPENAL=${HAVE_LIBOPENAL})
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

        message(STATUS "MediaPlayer libraries  : ${MEDIAPLAYER_LIBRARIES}")
        message(STATUS "MediaPlayer definitions: ${MEDIAPLAYER_DEFINITIONS}")
        message(STATUS "MediaPlayer flags      : ${MEDIAPLAYER_FLAGS}")

    endif()

endif()
