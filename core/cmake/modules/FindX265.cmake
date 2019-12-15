# - Try to find X265 library
# Once done this will define
#
#  X265_FOUND        - system has libx265
#  X265_INCLUDE_DIRS - the libx265 include directory
#  X265_LIBRARIES    - Link these to use libx265
#  X265_DEFINITIONS  - Compiler switches required for using libx265
#
#  Copyright (c) 2019-2020 Caulier Gilles <caulier dot gilles at gmail dot com>
#
#  Redistribution and use is allowed according to the terms of the New BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

IF(X265_LIBRARIES AND X265_INCLUDE_DIRS AND X265_DEFINITIONS)

    # in cache already
    SET(X265_FOUND TRUE)

ELSE()

    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    IF(NOT WIN32)

        FIND_PACKAGE(PkgConfig)

        IF(PKG_CONFIG_FOUND)

           PKG_CHECK_MODULES(PC_X265 x265)

        ENDIF()

    ENDIF()

    FIND_PATH(X265_INCLUDE_DIRS x265.h
        HINTS ${PC_X265_INCLUDE_DIRS}
    )

    FIND_LIBRARY(X265_LIBRARIES NAMES x265 libx265
        HINTS ${PC_X265_LIBRARY_DIRS}
    )

    SET(X265_DEFINITIONS ${PC_X265_CFLAGS_OTHER})

    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(X265
                                      REQUIRED_VARS X265_INCLUDE_DIRS X265_LIBRARIES
    )

    # show the variables only in the advanced view
    MARK_AS_ADVANCED(X265_INCLUDE_DIRS X265_LIBRARIES X265_DEFINITIONS)

ENDIF()

