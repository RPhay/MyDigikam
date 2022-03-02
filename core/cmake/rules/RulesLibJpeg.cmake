#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

DETECT_JPEG()
set(DIGIKAM_LIBJPEG_DIR libjpeg/${JPEG_LIB_VERSION})
message(STATUS "Using libjpeg sub-directory: ${DIGIKAM_LIBJPEG_DIR}")

# JPEG library version check

message(STATUS "Looking for LibJpeg Version...")

if(JPEG_FOUND)

    include_directories(${JPEG_INCLUDE_DIRS})

    # JPEG lib version is done on top level through DETECT_JPEG() from MacroJPEG.cmake

    if (${JPEG_LIB_VERSION} LESS 80)

        set(JPEG8_FOUND FALSE)

    else()

        set(JPEG8_FOUND TRUE)

    endif()

endif()
