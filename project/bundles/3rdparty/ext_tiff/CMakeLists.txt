# Script to build libgphoto for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_tiff "${EXTPREFIX}" )

ExternalProject_Add(ext_tiff
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://gitlab.com/libtiff/libtiff.git

    GIT_TAG v4.3.0

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix ${EXTPREFIX_tiff}
    UPDATE_COMMAND ""
    ALWAYS 0
)

