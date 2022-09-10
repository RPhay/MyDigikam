# Script to build libpng for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
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
