# Script to build AOM library for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2024 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

set(PREFIX_libaom "${EXTPREFIX}")

# NOTES:
# - Libaom is installed with Macports under MacOS

set(AOM_Conf -DENABLE_EXAMPLES=OFF
             -DENABLE_TESTS=OFF
             -DENABLE_TOOLS=OFF
             -DENABLE_DOCS=OFF
             -DENABLE_TESTDATA=OFF
             -DCONFIG_PIC=1
             -DBUILD_SHARED_LIBS=ON
)

if(MINGW)

    # See bugs #471797 and 471269
    set(AOM_Conf ${AOM_Conf}
               -DAOM_TARGET_CPU=x86_64
               -DAOM_TARGET_SYSTEM=Windows
               -DENABLE_MMX=ON
               -DENABLE_SSE=ON
               -DENABLE_SSE2=OFF
               -DENABLE_SSE3=OFF
               -DENABLE_SSE4_1=OFF
               -DENABLE_SSE4_2=OFF
               -DENABLE_SSSE3=OFF
    )

endif()

JoinListAsString("${AOM_Conf}" " " BASH_OPTIONS_AOM)

if(MINGW)

    ExternalProject_Add(ext_libaom

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://aomedia.googlesource.com/aom
        GIT_TAG v3.8.3

        CONFIGURE_COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo ${BASH_OPTIONS_AOM}

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        BUILD_IN_SOURCE 1

        INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE) install/fast

        UPDATE_COMMAND ""
        ALWAYS 0
    )

else() # Linux

    ExternalProject_Add(ext_libaom

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://aomedia.googlesource.com/aom
        GIT_TAG v3.8.3

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_libaom}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   ${GLOBAL_PROFILE}
                   ${AOM_Conf}

        BUILD_IN_SOURCE 0

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()
