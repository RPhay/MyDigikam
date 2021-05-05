# Script to build GMic-Qt for digiKam bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(PREFIX_ext_gmicqt "${EXTPREFIX}")

#if (APPLE)
#    set(FFTW_SUPPORT OFF)
#else()
    set(FFTW_SUPPORT ON)
#endif()

set(GmicQt_Conf -DGMIC_QT_HOST=digikam
                -DENABLE_ASAN=OFF
                -DENABLE_FFTW3=${FFTW_SUPPORT}
                ${GLOBAL_PROFILE}
)

JoinListAsString("${GmicQt_Conf}" " " BASH_OPTIONS)

# Download and build gmic-qt
if (MINGW)

    ExternalProject_Add(ext_gmic_qt

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/cgilles/gmic-qt.git

        SOURCE_DIR gmic-qt

        CONFIGURE_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo ${BASH_OPTIONS}

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && make install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
    )

else() # MacOS and Linux

    ExternalProject_Add(ext_gmic_qt

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/cgilles/gmic-qt.git

        SOURCE_DIR gmic-qt
        INSTALL_DIR ${PREFIX_ext_gmicqt}

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_gmic} -DCMAKE_BUILD_TYPE=RelWithDebInfo ${GmicQt_Conf}

        UPDATE_COMMAND ""
    )

endif()
