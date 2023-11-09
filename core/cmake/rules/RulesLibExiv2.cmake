#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find_package(LibExiv2 OPTIONAL_COMPONENTS)

if(LibExiv2_FOUND AND ("${LibExiv2_VERSION}" VERSION_GREATER "0.28.0"))

    find_package(LibExiv2 REQUIRED)

else()

    set (LibExiv2_FOUND FALSE)
    find_package(exiv2 REQUIRED)

endif()

set_package_properties("exiv2"     PROPERTIES
                       DESCRIPTION "Required to build digiKam"
                       URL         "https://www.exiv2.org"
                       TYPE        RECOMMENDED
                       PURPOSE     "Library to manage image metadata"
)

if(LibExiv2_FOUND)

    if("${LibExiv2_VERSION}" VERSION_LESS ${EXIV2_MIN_VERSION})

        message(FATAL_ERROR "Exiv2 version is too old (${LibExiv2_VERSION})! Minimal version required:${EXIV2_MIN_VERSION}.")

    endif()

    if("${LibExiv2_VERSION}" VERSION_LESS "0.27.99")

        set(EXIV2_CXX_STANDARD 11)

    else()

        set(EXIV2_CXX_STANDARD 17)

    endif()

else()

    if("${exiv2_VERSION}" VERSION_LESS ${EXIV2_MIN_VERSION})

        message(FATAL_ERROR "Exiv2 version is too old (${exiv2_VERSION})! Minimal version required:${EXIV2_MIN_VERSION}.")

    endif()

    if("${exiv2_VERSION}" VERSION_LESS "0.27.99")

        set(EXIV2_CXX_STANDARD 11)

    else()

        set(EXIV2_CXX_STANDARD 17)

    endif()

endif()
