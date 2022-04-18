# Script to build UcharDet for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
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
