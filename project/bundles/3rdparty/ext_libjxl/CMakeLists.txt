# Script to build jpegXL library for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(PREFIX_ext_libjxl "${EXTPREFIX}")

set(JpegXL_Conf -DBUILD_SHARED_LIBS=ON
                -DBUILD_TESTING=OFF
)

if(MINGW)

    # See bug comment https://bugs.kde.org/show_bug.cgi?id=436533#c26

    set(JpegXL_Conf ${JpegXL_Conf}
        -DCMAKE_CXX_FLAGS="-DHWY_COMPILE_ONLY_SCALAR"
    )

endif()

JoinListAsString("${JpegXL_Conf}" " " BASH_OPTIONS)

if(MINGW)

    ExternalProject_Add(ext_libjxl
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/libjxl/libjxl.git
        GIT_TAG v0.6.1

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/jxl-lib-mxe.patch

        CONFIGURE_COMMAND git submodule update --init --recursive --depth 1 --recommend-shallow &&
                          cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo ${BASH_OPTIONS}

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE) -j

        INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE) install/fast &&
                        cp -f <SOURCE_DIR>/build.mxe/third_party/brotli/libbrotlicommon.dll ${PREFIX_ext_libjxl}/bin &&
                        cp -f <SOURCE_DIR>/build.mxe/third_party/brotli/libbrotlidec.dll    ${PREFIX_ext_libjxl}/bin &&
                        cp -f <SOURCE_DIR>/build.mxe/third_party/brotli/libbrotlienc.dll    ${PREFIX_ext_libjxl}/bin &&
                        mv -f ${PREFIX_ext_libjxl}/lib/libjxl.dll                           ${PREFIX_ext_libjxl}/bin &&
                        mv -f ${PREFIX_ext_libjxl}/lib/libjxl_threads.dll                   ${PREFIX_ext_libjxl}/bin

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

elseif(APPLE)

    ExternalProject_Add(ext_libjxl
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        # To use official release tarball

        GIT_REPOSITORY https://github.com/libjxl/libjxl.git
        GIT_TAG v0.6.1

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/jxl-lib-macports.patch

        CONFIGURE_COMMAND git submodule update --init --recursive --depth 1 --recommend-shallow &&
                          cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.macports <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.macports ${PREFIX_ext_libjxl} RelWithDebInfo x86_64 ${BASH_OPTIONS}

        BUILD_COMMAND cd <SOURCE_DIR>/build && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build && $(MAKE) install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

else() # Linux

    ExternalProject_Add(ext_libjxl
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/libjxl/libjxl.git
        GIT_TAG v0.6.1

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/jxl-lib-linux.patch

        CONFIGURE_COMMAND git submodule update --init --recursive --depth 1 --recommend-shallow &&
                          $(CMAKE_COMMAND) . -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_libjxl}
                                             -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                                             -DICONV_INCLUDE_DIR=${PREFIX_ext_libjxl}/include
                                             ${JpegXL_Conf}
                                             ${GLOBAL_PROFILE}

        BUILD_COMMAND cd <SOURCE_DIR> && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR> && $(MAKE) install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()
