#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# NOTE: X11 API are used to manage Monitor Profiles with LCMS

find_package(X11)

if(X11_FOUND)

    if(NOT Qt6_FOUND)

        find_package(Qt5 ${QT_MIN_VERSION} NO_MODULE
                                           COMPONENTS
                                           X11Extras
        )

    endif()

    set(HAVE_X11 TRUE)

else()

    set(HAVE_X11 FALSE)

endif()
