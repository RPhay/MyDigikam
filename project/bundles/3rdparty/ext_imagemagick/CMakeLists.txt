# Script to build imagemagick for digiKam bundle.
#
# Copyright (c) 2015-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(PREFIX_ext_imagemagick "${EXTPREFIX}")

ExternalProject_Add(ext_imagemagick
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://github.com/ImageMagick/ImageMagick/archive/7.0.8-64.tar.gz
    URL_MD5 053dac22b86ef475259b6f8b952997df

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREFIX_ext_imagemagick}
                                             --without-freetype
                                             --without-fontconfig
                                             --without-raw
                                             --without-lqr
                                             --without-openjp2
                                             --without-threads
                                             --without-perl
                                             --without-x
                                             --disable-docs
                                             --disable-static
                                             --with-magick-plus-plus
                                             --with-heic
                                             --with-flif
                                             --with-jbig
                                             --with-webp
                                             --with-openexr
                                             --disable-hdri
                                             --with-quantum-depth=16
#                                             --with-utilities=no

    UPDATE_COMMAND ""
    ALWAYS 0
)
