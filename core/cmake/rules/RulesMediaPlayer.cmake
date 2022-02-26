#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(FFmpeg COMPONENTS AVCODEC AVDEVICE AVFILTER AVFORMAT AVUTIL SWSCALE)
find_package(QtAV)

if(ENABLE_MEDIAPLAYER)

    if(${AVCODEC_FOUND} AND ${AVDEVICE_FOUND} AND ${AVFILTER_FOUND} AND ${AVFORMAT_FOUND} AND ${AVUTIL_FOUND} AND ${SWSCALE_FOUND})

        include_directories(${FFMPEG_INCLUDE_DIRS})

    else()

        set(ENABLE_MEDIAPLAYER OFF)
        set(FFMPEG_FOUND OFF)
        message(STATUS "ENABLE_MEDIAPLAYER option is enabled but FFMpeg cannot be found. Media player support is disabled.")

    endif()

    if(NOT ${QtAV_FOUND} OR "${QTAV_VERSION_STRING}" VERSION_LESS ${QTAV_MIN_VERSION})

        set(ENABLE_MEDIAPLAYER OFF)
        set(QtAV_FOUND OFF)
        message(STATUS "ENABLE_MEDIAPLAYER option is enabled but QtAV cannot be found. Media player support is disabled.")

    else()

        include_directories(${QTAV_INCLUDE_DIRS})

    endif()

    if (${QtAV_FOUND} AND ${FFMPEG_FOUND})

        message(STATUS "Media player support is enabled.")

    endif()

endif()
