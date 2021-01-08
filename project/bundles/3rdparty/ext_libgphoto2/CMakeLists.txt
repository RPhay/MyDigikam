# Script to build libgphoto for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_libgphoto2 "${EXTPREFIX}")

ExternalProject_Add(ext_libgphoto2
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL http://downloads.sourceforge.net/project/gphoto/libgphoto/2.5.24/libgphoto2-2.5.25.tar.bz2
    URL_MD5 39999aa4bdd3bf849b5716153c659405

    INSTALL_DIR ${EXTPREFIX_libgphoto2}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix ${EXTPREFIX_libgphoto2}

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)
