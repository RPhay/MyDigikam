# Script to build breeze style for digiKam bundle.
#
# Copyright (c) 2015-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(EXTPREFIX_breeze "${EXTPREFIX}")

if(MINGW)

    GetKF5PlasmaMd5Sums(breeze ${KP_VERSION})

    ExternalProject_Add(ext_breeze

                        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
                        URL https://download.kde.org/stable/plasma/${KP_VERSION}/breeze-${KP_VERSION}.tar.xz
                        URL_MD5 ${KF5_PLASMA_MD5SUM}

                        INSTALL_DIR ${EXTPREFIX_breeze}
                        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/breeze-common-rules.patch &&
                                      cp -f ${CMAKE_CURRENT_SOURCE_DIR}/breezesettings.cpp <SOURCE_DIR>/kstyle/.    &&
                                      cp -f ${CMAKE_CURRENT_SOURCE_DIR}/breezesettings.h <SOURCE_DIR>/kstyle/.      &&
                                      cp -f ${CMAKE_CURRENT_SOURCE_DIR}/breezestyleconfigdata.cpp <SOURCE_DIR>/kstyle/.    &&
                                      cp -f ${CMAKE_CURRENT_SOURCE_DIR}/breezestyleconfigdata.h <SOURCE_DIR>/kstyle/.

                        CMAKE_ARGS -DMXE_TOOLCHAIN=${MXE_TOOLCHAIN}
                                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                                   -DCMAKE_COLOR_MAKEFILE=ON
                                   -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_breeze}
                                   -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON
                                   -DCMAKE_TOOLCHAIN_FILE=${MXE_TOOLCHAIN}
                                   -DCMAKE_FIND_PREFIX_PATH=${CMAKE_PREFIX_PATH}
                                   -DCMAKE_SYSTEM_INCLUDE_PATH=${CMAKE_PREFIX_PATH}/include
                                   -DCMAKE_INCLUDE_PATH=${CMAKE_PREFIX_PATH}/include
                                   -DCMAKE_LIBRARY_PATH=${CMAKE_PREFIX_PATH}/lib
                                   -DZLIB_ROOT=${CMAKE_PREFIX_PATH}
                                   ${GLOBAL_PROFILE}
                                   "${argoptions}"
                                   -DWITH_WALLPAPERS=OFF
                                   -DWITH_DECORATIONS=OFF
                                   -DBUILD_TESTING=OFF
                                   -Wno-dev

                        UPDATE_COMMAND ""
                        ALWAYS 0
    )

elseif(APPLE)

    GetKF5PlasmaMd5Sums(breeze ${KP_VERSION})

    ExternalProject_Add(ext_breeze

                        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
                        URL https://download.kde.org/stable/plasma/${KP_VERSION}/breeze-${KP_VERSION}.tar.xz
                        URL_MD5 ${KF5_PLASMA_MD5SUM}

                        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/breeze-common-rules.patch

                        CONFIGURE_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.macports <SOURCE_DIR>/. &&
                                          <SOURCE_DIR>/bootstrap.macports ${EXTPREFIX_breeze} debug x86_64 "-DWITH_WALLPAPERS=OFF -DWITH_DECORATIONS=OFF -DBUILD_TESTING=OFF -Wno-dev"

                        BUILD_COMMAND cd <SOURCE_DIR>/build && $(MAKE)

                        INSTALL_COMMAND cd <SOURCE_DIR>/build && make install/fast

                        BUILD_IN_SOURCE 1

                        UPDATE_COMMAND ""
                        ALWAYS 0
    )

else() # Linux

    GetKF5PlasmaMd5Sums(breeze ${KP_VERSION})

    ExternalProject_Add(ext_breeze

                        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
                        URL https://download.kde.org/stable/plasma/${KP_VERSION}/breeze-${KP_VERSION}.tar.xz
                        URL_MD5 ${KF5_PLASMA_MD5SUM}

                        INSTALL_DIR ${EXTPREFIX_breeze}

                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_breeze}
                                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                                   ${GLOBAL_PROFILE}
                                   -DCMAKE_SYSTEM_PREFIX_PATH=${EXTPREFIX}
                                   -DWITH_WALLPAPERS=OFF
                                   -DWITH_DECORATIONS=OFF
                                   -DBUILD_TESTING=OFF
                                   -Wno-dev

                        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/breeze-common-rules.patch

                        UPDATE_COMMAND ""
                        ALWAYS 0
    )

endif()
