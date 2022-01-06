# Script to install DrMinGw run-time for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(PREFIX_ext_drmingw "${EXTPREFIX}")

# Compile DrMingw source code

ExternalProject_Add(ext_drmingw

    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://github.com/jrfonseca/drmingw.git
    GIT_TAG 0.9.4

    INSTALL_DIR ${EXTPREFIX_drmingw}

    PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/drmingw-coreonly.patch

    CONFIGURE_COMMAND git submodule update --init &&
                      cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                      <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo -DBUILD_TESTING=OFF -DCMAKE_DISABLE_FIND_PACKAGE_PythonInterp=1 -DCMAKE_DISABLE_FIND_PACKAGE_WinDbg=1

    BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

    INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && make install/fast &&
                    ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/build.mxe/thirdparty/libz.dll ${PREFIX_ext_drmingw}/bin/

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)

# Deploying DrMingw run-time dependencies

ExternalProject_Add(ext_drmingw_deps
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://github.com/jrfonseca/drmingw/releases/download/0.9.3/drmingw-0.9.3-win64.7z
    URL_HASH MD5=f2a2e62a0214fe43525cf380d639b927

    INSTALL_DIR ${PREFIX_ext_drmingw}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/dbghelp.dll ${PREFIX_ext_drmingw}/bin/
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/dbgcore.dll ${PREFIX_ext_drmingw}/bin/
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/symsrv.dll ${PREFIX_ext_drmingw}/bin/
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/symsrv.yes ${PREFIX_ext_drmingw}/bin/

    UPDATE_COMMAND ""
)

add_dependencies(ext_drmingw ext_drmingw_deps)
