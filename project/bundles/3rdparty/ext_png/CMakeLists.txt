# Script to build libpng for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(PREFIX_ext_png "${EXTPREFIX}")

ExternalProject_Add(ext_png
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL http://prdownloads.sourceforge.net/libpng/libpng-1.6.37.tar.xz
    URL_MD5 015e8e15db1eecde5f2eb9eb5b6e59e9

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREFIX_ext_png}
    UPDATE_COMMAND ""
    ALWAYS 0
)
