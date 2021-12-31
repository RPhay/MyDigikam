# Script to build Fcitx-qt for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_fcitx-qt5 "${EXTPREFIX}" )

ExternalProject_Add(ext_fcitx-qt5
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://github.com/fcitx/fcitx-qt5.git
    GIT_TAG 1.2.6

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_fcitx-qt5}
               -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
               -DENABLE_LIBRARY=OFF
               ${GLOBAL_PROFILE}

    UPDATE_COMMAND ""
    ALWAYS 0
)
