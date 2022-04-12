#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(FFmpeg COMPONENTS AVCODEC
                               AVDEVICE
                               AVFILTER
                               AVFORMAT
                               AVUTIL
                               SWSCALE
)

if(ENABLE_MEDIAPLAYER)

    if(${AVCODEC_FOUND} AND ${AVDEVICE_FOUND} AND ${AVFILTER_FOUND} AND ${AVFORMAT_FOUND} AND ${AVUTIL_FOUND} AND ${SWSCALE_FOUND})

        include_directories(${FFMPEG_INCLUDE_DIRS})

    else()

        set(ENABLE_MEDIAPLAYER OFF)
        set(FFMPEG_FOUND OFF)
        message(STATUS "ENABLE_MEDIAPLAYER option is enabled but FFMpeg cannot be found. Media player support is disabled.")

    endif()

    if (${FFMPEG_FOUND})

        message(STATUS "Media player support is enabled.")

    endif()

endif()
