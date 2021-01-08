# Script to build MosaicWall for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(MosaicWall_Conf -DENABLE_DPLUGIN=ON
                    ${GLOBAL_PROFILE}
)

JoinListAsString("${MosaicWall_Conf}" " " BASH_OPTIONS)

SET(PREFIX_ext_mosaicwall "${EXTPREFIX}")

if (MINGW)

    ExternalProject_Add(ext_mosaicwall

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/scheckmedia/ImageMosaicWall.git

        CONFIGURE_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo ${BASH_OPTIONS}

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && make install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
    )

else() # MacOS and Linux

    ExternalProject_Add(ext_mosaicwall

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/scheckmedia/ImageMosaicWall.git

        INSTALL_DIR ${PREFIX_ext_mosaicwall}

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_mosaicwall} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_DPLUGIN=ON

        INSTALL_COMMAND cd <SOURCE_DIR>/dplugin && make install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
    )

endif()
