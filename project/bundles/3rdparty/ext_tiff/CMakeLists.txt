# Script to build libgphoto for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_tiff "${EXTPREFIX}" )

ExternalProject_Add(ext_tiff
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://gitlab.com/libtiff/libtiff/-/archive/Release-v4-1-0/libtiff-Release-v4-1-0.tar.bz2
    URL_MD5 7de199c736c3469d94cbb6c1a267cff4

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix ${EXTPREFIX_tiff}
    UPDATE_COMMAND ""
    ALWAYS 0
)

