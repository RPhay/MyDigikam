# Script to build FlowView plugin for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

SET(PREFIX_ext_flowview "${EXTPREFIX}")

if (MINGW)

    ExternalProject_Add(ext_flowview

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/cathaysia/digikamflowplugin.git

        PATCH_COMMAND git reset --hard &&
                      ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/fix-c++-qdebug.patch

        CONFIGURE_COMMAND git submodule update --init &&
                          cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && make install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
    )

else() # MacOS and Linux

    ExternalProject_Add(ext_flowview

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/cathaysia/digikamflowplugin.git

        PATCH_COMMAND git reset --hard &&
                      ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/fix-c++-qdebug.patch &&
                      git submodule update --init

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_flowview} -DCMAKE_BUILD_TYPE=RelWithDebInfo

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
    )

endif()
