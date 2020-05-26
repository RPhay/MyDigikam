# Script to build MosaicWall for digiKam bundle.
#
# Copyright (c) 2015-2020 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(PREFIX_ext_mosaicwall "${EXTPREFIX}")

ExternalProject_Add(ext_mosaicwall

    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    GIT_REPOSITORY https://github.com/scheckmedia/ImageMosaicWall.git
#    GIT_REPOSITORY https://github.com/cgilles/ImageMosaicWall.git

    INSTALL_DIR ${PREFIX_ext_mosaicwall}

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_mosaicwall} -DCMAKE_BUILD_TYPE=RelWithDebInfo

    UPDATE_COMMAND ""
)
