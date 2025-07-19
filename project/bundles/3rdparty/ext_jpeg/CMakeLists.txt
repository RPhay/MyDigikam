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
    URL https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/3.1.1/libjpeg-turbo-3.1.1.tar.gz
    URL_MD5 e71063e0b043e38207c8cefda6931cbd

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREFIX_ext_jpeg}
                                             --without-simd
                                             --with-jpeg8

    UPDATE_COMMAND ""
    ALWAYS 0
)
