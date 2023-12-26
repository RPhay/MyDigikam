# Script to build MosaicWall plugin for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2024 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(MosaicWall_Conf -DENABLE_DPLUGIN=ON
                    ${GLOBAL_PROFILE}
)

if(${ENABLE_QTVERSION} MATCHES "6")

    set(MosaicWall_Conf ${MosaicWall_Conf} -DBUILD_WITH_QT6=ON)

else()

    set(MosaicWall_Conf ${MosaicWall_Conf} -DBUILD_WITH_QT6=OFF)

endif()

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

else() # MacOS, Linux, and VCPKG

    ExternalProject_Add(ext_mosaicwall

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/scheckmedia/ImageMosaicWall.git

        INSTALL_DIR ${PREFIX_ext_mosaicwall}

        PATCH_COMMAND git reset --hard &&
                      ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/mosaicwall-exiv2-cpp17.patch

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_mosaicwall} -DCMAKE_BUILD_TYPE=RelWithDebInfo ${MosaicWall_Conf}

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
    )

endif()
