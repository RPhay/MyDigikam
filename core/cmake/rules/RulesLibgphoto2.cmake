#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(Gphoto2)

if(Gphoto2_FOUND)

    if("${GPHOTO2_VERSION_STRING}" VERSION_GREATER "2.4.0")

        set(VERSION_GPHOTO2 true)

    else()

        set(VERSION_GPHOTO2 false)

    endif()

    if("${GPHOTO2_VERSION_STRING}" VERSION_GREATER "2.5.0")

        set(VERSION_GPHOTO25 true)
        message(STATUS "libgphoto2 API version >= 2.5")

    else()

        set(VERSION_GPHOTO25 false)
        message(STATUS "libgphoto2 API version < 2.5")

    endif()

    if(VERSION_GPHOTO25)

        set(HAVE_GPHOTO25 1)

    else()

        set(HAVE_GPHOTO25 0)

    endif()

endif()
