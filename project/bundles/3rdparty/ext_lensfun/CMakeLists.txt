# Script to build Lensfun for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

SET(EXTPREFIX_lensfun "${EXTPREFIX}" )

set(Lensfun_Conf -DBUILD_STATIC=OFF
                 -DBUILD_WITH_MSVC_STATIC_RUNTIME=OFF
                 -DBUILD_TESTS=OFF
                 -DBUILD_LENSTOOL=OFF
                 -DBUILD_DOC=OFF
                 -DINSTALL_PYTHON_MODULE=OFF
                 -DINSTALL_HELPER_SCRIPTS=OFF
)

JoinListAsString("${Lensfun_Conf}" " " BASH_OPTIONS)

if (MINGW)

    ExternalProject_Add(ext_lensfun

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/lensfun/lensfun.git
        GIT_TAG v0.3.3

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/lensfun_regex_mxe.patch

        CONFIGURE_COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/create_manifest.sh <SOURCE_DIR>/. &&
                          cd <SOURCE_DIR> &&
                          <SOURCE_DIR>/create_manifest.sh ${EXTERNALS_DOWNLOAD_DIR} lensfun &&
                          cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo ${BASH_OPTIONS}

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && make install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

elseif (APPLE)

    ExternalProject_Add(ext_lensfun

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/lensfun/lensfun.git
        GIT_TAG v0.3.3

        INSTALL_DIR ${EXTPREFIX_lensfun}

        CONFIGURE_COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/create_manifest.sh <SOURCE_DIR>/. &&
                          cd <SOURCE_DIR> &&
                          <SOURCE_DIR>/create_manifest.sh ${EXTERNALS_DOWNLOAD_DIR} lensfun &&
                          cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.macports <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.macports ${EXTPREFIX_lensfun} RelWithDebInfo x86_64 ${BASH_OPTIONS}

        BUILD_COMMAND cd <SOURCE_DIR>/build && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build && $(MAKE) install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

else() # Linux

    ExternalProject_Add(ext_lensfun

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/lensfun/lensfun.git
        GIT_TAG v0.3.3

        INSTALL_DIR ${EXTPREFIX_lensfun}

        PATCH_COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/create_manifest.sh <SOURCE_DIR>/. &&
                      cd <SOURCE_DIR> &&
                      <SOURCE_DIR>/create_manifest.sh ${EXTERNALS_DOWNLOAD_DIR} lensfun

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_lensfun}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   ${Lensfun_Conf}
                   ${GLOBAL_PROFILE}

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()
