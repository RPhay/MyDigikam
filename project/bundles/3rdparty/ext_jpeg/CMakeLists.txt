# Script to build libjpeg for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2025 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

SET(PREFIX_ext_jpeg "${EXTPREFIX}")

ExternalProject_Add(ext_jpeg
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/3.1.2/libjpeg-turbo-3.1.2.tar.gz
    URL_MD5 6f4780b95edde615c34de515f5eb617a

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREFIX_ext_jpeg}
                                             --without-simd
                                             --with-jpeg8

    UPDATE_COMMAND ""
    ALWAYS 0
)
