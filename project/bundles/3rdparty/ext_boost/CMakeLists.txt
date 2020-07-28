# Script to build Boost for digiKam bundle.
#
# Copyright (c) 2015-2020 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(PREFIX_ext_boost "${EXTPREFIX}")

ExternalProject_Add( ext_boost
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL http://downloads.sourceforge.net/project/boost/boost/1.73.0/boost_1_73_0.tar.bz2
    URL_MD5 9273c8c4576423562bbe84574b07b2bd
    CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap.sh --prefix=${PREFIX_ext_boost} --with-libraries=system
    BUILD_COMMAND <SOURCE_DIR>/b2 install
    INSTALL_COMMAND ""
    INSTALL_DIR ${PREFIX_ext_boost}

    UPDATE_COMMAND ""
    ALWAYS 0
    BUILD_IN_SOURCE 1
)
