#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

message(STATUS "")
message(STATUS "--------------------------------------------------")
message(STATUS "digiKam MediaPlayer dependencies checks:")
message(STATUS "")

find_package(FFmpeg COMPONENTS AVCODEC
                               AVDEVICE
                               AVFILTER
                               AVFORMAT
                               AVUTIL
                               SWSCALE
                               AVRESAMPLE
                               SWRESAMPLE
)

find_package(ASS)
find_package(OpenAL)
find_package(Portaudio)
find_package(PulseAudio)
find_package(VAAPI)

if(ENABLE_MEDIAPLAYER)

    message(STATUS "FFMpeg AVCodec    : ${AVCODEC_FOUND}")
    message(STATUS "FFMpeg AVDevice   : ${AVDEVICE_FOUND}")
    message(STATUS "FFMpeg AVFilter   : ${AVFILTER_FOUND}")
    message(STATUS "FFMpeg AVFormat   : ${AVFORMAT_FOUND}")
    message(STATUS "FFMpeg AVUtil     : ${AVUTIL_FOUND}")
    message(STATUS "FFMpeg SWScale    : ${SWSCALE_FOUND}")
    message(STATUS "FFMpeg AVResample : ${AVRESAMPLE_FOUND}")
    message(STATUS "FFMpeg SWResample : ${SWRESAMPLE_FOUND}")

    if(${AVCODEC_FOUND}    AND
       ${AVDEVICE_FOUND}   AND
       ${AVFILTER_FOUND}   AND
       ${AVFORMAT_FOUND}   AND
       ${AVUTIL_FOUND}     AND
       ${SWSCALE_FOUND}    AND
       ${AVRESAMPLE_FOUND} AND
       ${SWRESAMPLE_FOUND}
      )

        include_directories(${FFMPEG_INCLUDE_DIRS})
        message(STATUS "MediaPlayer support is enabled                      : yes")

    else()

        set(ENABLE_MEDIAPLAYER OFF)
        set(FFMPEG_FOUND OFF)
        message(STATUS "MediaPlayer support is enabled                      : no")

    endif()

    if (${FFMPEG_FOUND})

        MACRO_BOOL_TO_01(ASS_FOUND        HAVE_LIBASS)
        MACRO_BOOL_TO_01(OPENAL_FOUND     HAVE_LIBOPENAL)
        MACRO_BOOL_TO_01(PORTAUDIO_FOUND  HAVE_LIBPORTAUDIO)
        MACRO_BOOL_TO_01(PULSEAUDIO_FOUND HAVE_LIBPULSEAUDIO)
        MACRO_BOOL_TO_01(VAAPI_FOUND      HAVE_LIBVAAPI)
        MACRO_BOOL_TO_01(AVCODEC_FOUND    HAVE_LIBAVCODEC)
        MACRO_BOOL_TO_01(AVDEVICE_FOUND   HAVE_LIBAVDEVICE)
        MACRO_BOOL_TO_01(AVFILTER_FOUND   HAVE_LIBAVFILTER)
        MACRO_BOOL_TO_01(AVFORMAT_FOUND   HAVE_LIBAVFORMAT)
        MACRO_BOOL_TO_01(AVUTIL_FOUND     HAVE_LIBAVUTIL)
        MACRO_BOOL_TO_01(SWSCALE_FOUND    HAVE_LIBSWSCALE)
        MACRO_BOOL_TO_01(AVRESAMPLE_FOUND HAVE_LIBAVRESAMPLE)
        MACRO_BOOL_TO_01(SWRESAMPLE_FOUND HAVE_LIBSWRESAMPLE)

        set(MEDIAPLAYER_LIBRARIES ${FFMPEG_LIBRARIES} ${CMAKE_DL_LIBS})

        if(ASS_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${ASS_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with LibASS support    : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with LibASS support    : no")

        endif()

        if(OPENAL_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENAL_LIBRARY})
            message(STATUS "MediaPlayer will be compiled with OpenAL support    : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with OpenAL support    : no")

        endif()

        if(PORTAUDIO_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${PORTAUDIO_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with PortAudio support : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with PortAudio support : no")

        endif()

        if(PULSEAUDIO_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${PULSEAUDIO_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with PulseAudio support: yes")

        else()

            message(STATUS "MediaPlayer will be compiled with PukseAudio support: no")

        endif()

        if(VAAPI_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${VAAPI_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with VaAPI support     : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with VaAPI support     : no")

        endif()

        if(OPENGL_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENGL_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGL)

            if(Qt6_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

            endif()
            message(STATUS "MediaPlayer will be compiled with OpenGL support    : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with OpenGL support    : no")

        endif()

        message(STATUS "MediaPlayer libraries: ${MEDIAPLAYER_LIBRARIES}")

    endif()

endif()
