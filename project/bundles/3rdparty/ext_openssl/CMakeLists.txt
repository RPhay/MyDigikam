# Script to build static version of OpenSSL for AppImage bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (NOT MINGW AND NOT APPLE)

    SET(EXTPREFIX_openssl "/opt/openssl")

    ExternalProject_Add(ext_openssl

        GIT_REPOSITORY https://github.com/flagarde/openssl-cmake.git

        INSTALL_DIR ${EXTPREFIX_openssl}

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_openssl}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE
                   -DWITH_APPS=OFF
                   -DWITH_DOCS=OFF
                   -DWITH_TESTS=OFF
                   ${GLOBAL_PROFILE}

        BUILD_COMMAND ${BUILD_CLI}

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()
