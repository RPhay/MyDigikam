#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find_package(exiv2 REQUIRED)

set_package_properties("exiv2"     PROPERTIES
                       DESCRIPTION "Required to build digiKam"
                       URL         "https://www.exiv2.org"
                       TYPE        RECOMMENDED
                       PURPOSE     "Library to manage image metadata"
)

if("${exiv2_VERSION}" VERSION_LESS ${EXIV2_MIN_VERSION})

    message(FATAL_ERROR "Exiv2 version is too old (${exiv2_VERSION})! Minimal version required:${EXIV2_MIN_VERSION}.")

endif()

if("${exiv2_VERSION}" VERSION_LESS "0.27.99")

    set(EXIV2_CXX_STANDARD 11)

else()

    set(EXIV2_CXX_STANDARD 17)

endif()
