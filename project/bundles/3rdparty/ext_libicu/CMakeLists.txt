# Script to build libicu for digiKam bundle.
#
# Copyright (c) 2015-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_libicu "${EXTPREFIX}")

ExternalProject_Add(ext_libicu
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://github.com/unicode-org/icu/archive/release-58-2.tar.gz
    URL_MD5 005466b3a8415020fcdef01961c9fdb9

    INSTALL_DIR ${EXTPREFIX_libicu}

    CONFIGURE_COMMAND cd <SOURCE_DIR>/icu4c/source && ./configure --prefix ${EXTPREFIX_libicu} --enable-tests=no --enable-samples=no --with-data-packaging=library --disable-renaming

    BUILD_COMMAND cd <SOURCE_DIR>/icu4c/source && $(MAKE)

    INSTALL_COMMAND cd <SOURCE_DIR>/icu4c/source && $(MAKE) install

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)
