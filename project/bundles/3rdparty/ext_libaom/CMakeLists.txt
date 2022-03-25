# Script to build AOM library for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(PREFIX_libaom "${EXTPREFIX}")

# NOTES:
# - Special MinGW rules to prevent mixed libaom versions hosted in bundles, for exemple with ffmpeg: build libaom as static library.
#   See this comment from Bugzilla for details: https://bugs.kde.org/show_bug.cgi?id=109060#c30
#
# - Libaom is installed with Macports under MacOS


set(AOM_Conf -DENABLE_EXAMPLES=OFF
             -DENABLE_TESTS=OFF
             -DENABLE_TOOLS=OFF
             -DENABLE_DOCS=OFF
             -DENABLE_TESTDATA=OFF
             -DCONFIG_PIC=1
)

if(MINGW)

    set(AOM_Conf ${AOM_Conf}
                 -DBUILD_SHARED_LIBS=OFF
    )

else()

    set(AOM_Conf ${AOM_Conf}
                 -DBUILD_SHARED_LIBS=ON
    )

endif()

JoinListAsString("${AOM_Conf}" " " BASH_OPTIONS_AOM)

if(MINGW)

    ExternalProject_Add(ext_libaom

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://aomedia.googlesource.com/aom
        GIT_TAG v3.3.0

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
        GIT_TAG v3.3.0

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_libaom}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   ${GLOBAL_PROFILE}
                   ${AOM_Conf}

        BUILD_IN_SOURCE 0

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()
