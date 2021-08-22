# Script to build libicu for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_libicu "${EXTPREFIX}/local")

ExternalProject_Add(ext_libicu
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://github.com/unicode-org/icu/archive/refs/tags/release-69-1.tar.gz
    URL_MD5 135125f633864285d637db5c01e0388b

    CONFIGURE_COMMAND cd <SOURCE_DIR>/icu4c/source &&
                      ./configure --prefix ${EXTPREFIX_libicu}
                                  --enable-tests=no
                                  --enable-samples=no
                                  --with-data-packaging=library
                                  --disable-renaming

    BUILD_COMMAND cd <SOURCE_DIR>/icu4c/source && $(MAKE)

    INSTALL_COMMAND cd <SOURCE_DIR>/icu4c/source && $(MAKE) install

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)
