# Script to build libpng for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2025 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

SET(PREFIX_ext_png "${EXTPREFIX}")

ExternalProject_Add(ext_png
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL http://prdownloads.sourceforge.net/libpng/libpng-1.6.50.tar.xz
    URL_MD5 4a6433f54317b8f0d4cb749c09d4eff2

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREFIX_ext_png}
    UPDATE_COMMAND ""
    ALWAYS 0
)
