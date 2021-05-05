# Script to install DrMinGw run-time for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(PREFIX_ext_drmingw "${EXTPREFIX}")

# Compile DrMingw source code

ExternalProject_Add(ext_drmingw
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://github.com/jrfonseca/drmingw/archive/0.9.2.tar.gz
    URL_MD5 08f6ab4587deeea2406a69a115d7a8c7

    INSTALL_DIR ${EXTPREFIX_drmingw}

    PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/drmingw-coreonly.patch

    CONFIGURE_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                      <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo -DBUILD_TESTING=OFF

    BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

    INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && make install/fast

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)

# Deploying DrMingw run-time dependencies

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

    # 64 bits

    ExternalProject_Add(ext_drmingw_deps
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://github.com/jrfonseca/drmingw/releases/download/0.9.2/drmingw-0.9.2-win64.7z
        URL_HASH MD5=687f29c0c6dc961890e7b4277a87c8a2

        INSTALL_DIR ${PREFIX_ext_drmingw}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/dbghelp.dll ${PREFIX_ext_drmingw}/bin/
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/symsrv.dll ${PREFIX_ext_drmingw}/bin/
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/symsrv.yes ${PREFIX_ext_drmingw}/bin/

        UPDATE_COMMAND ""
   )

else()

    # 32 bits

    ExternalProject_Add(ext_drmingw_deps
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://github.com/jrfonseca/drmingw/releases/download/0.9.2/drmingw-0.9.2-win32.7z
        URL_HASH MD5=78783e43b3509041b86597add2b8c028

        INSTALL_DIR ${PREFIX_ext_drmingw}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/dbghelp.dll ${PREFIX_ext_drmingw}/bin/
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/symsrv.dll ${PREFIX_ext_drmingw}/bin/
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/bin/symsrv.yes ${PREFIX_ext_drmingw}/bin/

        UPDATE_COMMAND ""
    )

endif()

add_dependencies(ext_drmingw ext_drmingw_deps)
