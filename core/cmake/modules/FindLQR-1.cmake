#  Try to find the lqr-1 library
#  Once done this will define
#
#  LQR-1_FOUND        - System has the lqr-1 library
#  LQR-1_INCLUDE_DIRS - The lqr-1 library include directory
#  LQR-1_LIBRARIES    - Link these to use the lqr-1 library
#
# Copyright (c) 2009      by Pino Toscano <pino at kde dot org>
# Copyright (c) 2012-2022 by Caulier Gilles <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if(LQR-1_LIBRARIES AND LQR-1_INCLUDE_DIRS)

    # In cache already

    set(LQR-1_FOUND TRUE)

else()

    if(NOT WIN32)

        if(NOT PKG_CONFIG_FOUND)

            include(FindPkgConfig)

        endif()

        pkg_check_modules(LQR-1 lqr-1)

    endif()

    if(LQR-1_LIBRARIES AND LQR-1_INCLUDE_DIRS)

        find_package(GLIB2)

        if(GLIB2_FOUND)

            include(CheckCXXSourceCompiles)
            set(CMAKE_REQUIRED_INCLUDES "${LQR-1_INCLUDE_DIRS}" "${GLIB2_INCLUDE_DIRS}")

            CHECK_CXX_SOURCE_COMPILES("
#include <lqr.h>

int main()
{
LqrImageType t = LQR_RGB_IMAGE;
return 0;
}
" HAVE_LQR_0_4)

            if(HAVE_LQR_0_4)

                include_directories(${GLIB2_INCLUDE_DIRS})
                set(LQR-1_FOUND TRUE)

            endif()

        endif()

    endif()

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(LQR-1 DEFAULT_MSG LQR-1_INCLUDE_DIRS LQR-1_LIBRARIES)

    # Show the LQR-1_INCLUDE_DIRS and LQR-1_LIBRARIES variables only in the advanced view

    mark_as_advanced(LQR-1_INCLUDE_DIRS LQR-1_LIBRARIES)

    message(STATUS "LQR-1_FOUND        = ${LQR-1_FOUND}")
    message(STATUS "LQR-1_INCLUDE_DIRS = ${LQR-1_INCLUDE_DIRS}")
    message(STATUS "LQR-1_LIBRARIES    = ${LQR-1_LIBRARIES}")

endif()
