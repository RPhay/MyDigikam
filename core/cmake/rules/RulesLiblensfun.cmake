#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(LensFun)

if(LENSFUN_VERSION)

    message(STATUS "liblensfun: Found version ${LENSFUN_VERSION} (required: ${LENSFUN_MIN_VERSION})")

    if(${LENSFUN_VERSION} VERSION_LESS ${LENSFUN_MIN_VERSION})

        set(LensFun_FOUND FALSE)

    endif()

else()

    message(STATUS "liblensfun: Version information not found, your version is probably too old.")
    set(LensFun_FOUND FALSE)

endif()
