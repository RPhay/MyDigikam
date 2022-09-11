# Script to build libjpeg for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

SET(PREFIX_ext_jpeg "${EXTPREFIX}")

ExternalProject_Add(ext_jpeg
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://sourceforge.net/projects/libjpeg-turbo/files/1.5.3/libjpeg-turbo-1.5.3.tar.gz
    URL_MD5 7c82f0f6a3130ec06b8a4d0b321cbca3

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREFIX_ext_jpeg}
                                             --without-simd
                                             --with-jpeg8

    UPDATE_COMMAND ""
    ALWAYS 0
)
