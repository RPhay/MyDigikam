#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

DETECT_OPENCV(${OPENCV_MIN_VERSION} core
                                    objdetect
                                    imgproc
                                    imgcodecs
                                    dnn
                                    flann
)

if(${OpenCV_FOUND})

    if(${OpenCV_VERSION} VERSION_LESS ${OPENCV_MIN_VERSION})

        message(STATUS "OpenCV < ${OPENCV_MIN_VERSION} have been found. Please use a more recent version.")
        set(OpenCV_FOUND FALSE)

    endif()

endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/app/utils/digikam_opencv.h.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/app/utils/digikam_opencv.h)

include_directories(${OpenCV_INCLUDE_DIRS})
