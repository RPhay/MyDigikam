# Script to build jasperlib for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

SET(PREFIX_ext_jasper "${EXTPREFIX}")

ExternalProject_Add(ext_jasper
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://github.com/jasper-software/jasper.git
    GIT_TAG version-2.0.33

    INSTALL_DIR ${PREFIX_ext_jasper}

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_jasper}
               -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
               ${GLOBAL_PROFILE}
               -DCMAKE_SYSTEM_PREFIX_PATH=${PREFIX_ext_jasper}
               -DJAS_ENABLE_SHARED=ON
               -DJAS_ENABLE_LIBJPEG=ON
               -DJAS_ENABLE_OPENGL=ON
               -DJAS_LOCAL=OFF
               -DJAS_ENABLE_DOC=OFF
               -DJAS_ENABLE_PROGRAMS=OFF
               -Wno-dev

    UPDATE_COMMAND ""
    ALWAYS 0
)
