# Script to build Cmake.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

SET(EXTPREFIX_cmake "${EXTPREFIX}")

ExternalProject_Add(ext_cmake

    GIT_REPOSITORY https://gitlab.kitware.com/cmake/cmake.git
    GIT_TAG v3.22.2

    INSTALL_DIR ${EXTPREFIX_cmake}

    CONFIGURE_COMMAND cd <SOURCE_DIR> && ./bootstrap --prefix=${EXTPREFIX_cmake}

    BUILD_COMMAND ${MAKE}

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)
