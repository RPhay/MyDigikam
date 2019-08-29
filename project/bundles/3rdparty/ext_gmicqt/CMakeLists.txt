# Script to build GMic-Qt for digiKam bundle.
#
# Copyright (c) 2015-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(PREFIX_ext_gmicqt "${EXTPREFIX}" )

# Download the gmic sources
ExternalProject_Add(ext_gmic_base
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://github.com/dtschump/gmic.git

    SOURCE_DIR gmic

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_gmicqt} -D -DCMAKE_BUILD_TYPE=Release ${GLOBAL_PROFILE}

    BUILD_COMMAND ""
    INSTALL_COMMAND ""

    BUILD_IN_SOURCE 1
)

# Download and build gmic-qt
ExternalProject_Add(ext_gmic_qt

    DEPENDS ext_gmic_base

    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    GIT_REPOSITORY https://github.com/cgilles/gmic-qt.git

    SOURCE_DIR gmic-qt
    INSTALL_DIR ${PREFIX_ext_gmicqt}

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_gmic} -DGMIC_QT_HOST=digikam -DCMAKE_BUILD_TYPE=Release ${GLOBAL_PROFILE}

    UPDATE_COMMAND ""
)
