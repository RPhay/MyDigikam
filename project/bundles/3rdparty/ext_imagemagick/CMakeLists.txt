# Script to build imagemagick for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(PREFIX_ext_imagemagick "${EXTPREFIX}")

ExternalProject_Add(ext_imagemagick
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://github.com/ImageMagick/ImageMagick.git
    GIT_TAG 7.1.0-27

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREFIX_ext_imagemagick}
                                             --without-freetype                     # TrueType font rendering library.
                                             --without-fontconfig                   # Disable configuring and customizing font access.
                                             --without-pango                        # Disable Layout and rendering of internationalized text.
                                             --without-raqm                         # Disable Complex Textlayout Library.
                                             --without-gvc                          # Disable rich set of graph drawing tools.
                                             --without-raw                          # Disable RAW codec.
                                             --without-lqr                          # Disable Liquid Rescale.
                                             --without-openjp2                      # Disable JPEG2000 codec.
                                             --without-heic                         # Disable HEIF codec.
                                             --without-jxl                          # Disable JPEG-XL codec.
                                             --without-threads                      # Disable internal threads.
                                             --without-perl                         # Disable Perl scripts.
                                             --without-x                            # Disable X11 support.
                                             --without-lcms                         # Disable Colors Management.
                                             --disable-docs                         # Disable documentations.
                                             --disable-static                       # Compile shared libraries.
                                             --disable-hdri                         # Disable HDRI support.
                                             --with-magick-plus-plus                # Enable C++ API.
                                             --with-flif                            # Enable JLIF codec.
                                             --with-jbig                            # Enable Big JPEG codec.
                                             --with-webp                            # Enable WebP codec.
                                             --with-openexr                         # Enable OpenEXR codec.
                                             --with-quantum-depth=16                # Use 1§Bits quantum for color components.
                                             --without-utilities                    # Disable CLI tools.

    UPDATE_COMMAND ""
    ALWAYS 0
)
