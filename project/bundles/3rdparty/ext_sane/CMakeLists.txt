# Script to build sane backends for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_sane "${EXTPREFIX}")

ExternalProject_Add(ext_sane
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://gitlab.com/sane-project/backends/-/archive/RELEASE_1_0_29/backends-RELEASE_1_0_29.tar.bz2
    URL_MD5 2aed6c56cee6893ade99071edcc4889f

    INSTALL_DIR ${EXTPREFIX_sane}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix ${EXTPREFIX_sane} --with-gphoto2=no --with-v4l=no

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)
