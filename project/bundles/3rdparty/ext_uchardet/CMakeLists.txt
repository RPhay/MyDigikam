# Script to build UcharDet for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

SET(EXTPREFIX_uchardet "${EXTPREFIX}")

ExternalProject_Add(ext_uchardet
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://github.com/freedesktop/uchardet
    GIT_TAG v0.0.7

    INSTALL_DIR ${EXTPREFIX_uchardet}

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_uchardet}
               -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
               ${GLOBAL_PROFILE}

    UPDATE_COMMAND ""
    ALWAYS 0
)
