# Script to build libheif for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(PREFIX_ext_heif "${EXTPREFIX}")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libde265.pc.in_cmake ${CMAKE_BINARY_DIR}/libde265.pc)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libheif.pc.in_cmake ${CMAKE_BINARY_DIR}/libheif.pc)

if(MINGW)

    ExternalProject_Add(de265

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://github.com/strukturag/libde265/archive/v1.0.3.tar.gz
        URL_MD5 189b4deb376ff879f8cbb2b7ffc24f97

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/de265-lib-mxe.patch

        CONFIGURE_COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        BUILD_IN_SOURCE 1

        INSTALL_COMMAND mkdir -p ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <SOURCE_DIR>/build.mxe/libde265/de265-version.h ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <SOURCE_DIR>/libde265/de265.h ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <SOURCE_DIR>/build.mxe/libde265/libde265.dll ${PREFIX_ext_heif}/bin/ &&
                        cp -fv <SOURCE_DIR>/build.mxe/libde265/libde265.dll.a ${PREFIX_ext_heif}/lib/ &&
                        cp -fv ${CMAKE_BINARY_DIR}/libde265.pc ${PREFIX_ext_heif}/lib/pkgconfig/

        UPDATE_COMMAND ""
        ALWAYS 0
    )

    ExternalProject_Add(ext_heif
        DEPENDS de265

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://github.com/strukturag/libheif/archive/v1.4.0.tar.gz
        URL_MD5 05dea500c105289c74270791cb5f8942

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/heif-lib-mxe.patch

        CONFIGURE_COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        BUILD_IN_SOURCE 1

        INSTALL_COMMAND mkdir -p ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_version.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_cxx.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_plugin.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/build.mxe/libheif/libheif.dll ${PREFIX_ext_heif}/bin/ &&
                        cp -fv <SOURCE_DIR>/build.mxe/libheif/libheif.dll.a ${PREFIX_ext_heif}/lib/ &&
                        cp -fv ${CMAKE_BINARY_DIR}/libheif.pc ${PREFIX_ext_heif}/lib/pkgconfig/

        UPDATE_COMMAND ""
        ALWAYS 0
    )


elseif(APPLE)

    ExternalProject_Add(de265

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        # NOTE: v1.0.3 do not compile under MacOS
        GIT_REPOSITORY https://github.com/strukturag/libde265.git

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_heif}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   ${GLOBAL_PROFILE}

        INSTALL_COMMAND mkdir -p ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <BINARY_DIR>/libde265/de265-version.h ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <SOURCE_DIR>/libde265/de265.h ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <BINARY_DIR>/libde265/libde265.dylib ${PREFIX_ext_heif}/lib/ &&
                        cp -fv ${CMAKE_BINARY_DIR}/libde265.pc ${PREFIX_ext_heif}/lib/pkgconfig/

        UPDATE_COMMAND ""
        ALWAYS 0
    )

    ExternalProject_Add(ext_heif
        DEPENDS de265

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://github.com/strukturag/libheif/archive/v1.4.0.tar.gz
        URL_MD5 05dea500c105289c74270791cb5f8942

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/heif-lib-macports.patch

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_heif}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   ${GLOBAL_PROFILE}

        INSTALL_COMMAND mkdir -p ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_version.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_cxx.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_plugin.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <BINARY_DIR>/libheif/libheif.dylib ${PREFIX_ext_heif}/lib/ &&
                        cp -fv ${CMAKE_BINARY_DIR}/libheif.pc ${PREFIX_ext_heif}/lib/pkgconfig/

        UPDATE_COMMAND ""
        ALWAYS 0
    )

else() # Linux

    ExternalProject_Add(de265

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://github.com/strukturag/libde265/archive/v1.0.3.tar.gz
        URL_MD5 189b4deb376ff879f8cbb2b7ffc24f97

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/de265-lib-linux.patch

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_heif}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   ${GLOBAL_PROFILE}

        INSTALL_COMMAND mkdir -p ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <BINARY_DIR>/libde265/de265-version.h ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <SOURCE_DIR>/libde265/de265.h ${PREFIX_ext_heif}/include/libde265/ &&
                        cp -fv <BINARY_DIR>/libde265/libde265.so ${PREFIX_ext_heif}/lib/ &&
                        cp -fv ${CMAKE_BINARY_DIR}/libde265.pc ${PREFIX_ext_heif}/lib/pkgconfig/

        UPDATE_COMMAND ""
        ALWAYS 0
    )

    ExternalProject_Add(ext_heif
        DEPENDS de265

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://github.com/strukturag/libheif/archive/v1.4.0.tar.gz
        URL_MD5 05dea500c105289c74270791cb5f8942

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_heif}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   ${GLOBAL_PROFILE}

        INSTALL_COMMAND mkdir -p ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_version.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_cxx.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <SOURCE_DIR>/libheif/heif_plugin.h ${PREFIX_ext_heif}/include/libheif/ &&
                        cp -fv <BINARY_DIR>/libheif/libheif.so ${PREFIX_ext_heif}/lib/ &&
                        cp -fv ${CMAKE_BINARY_DIR}/libheif.pc ${PREFIX_ext_heif}/lib/pkgconfig/

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()
