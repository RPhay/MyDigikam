# Script to build GMic-Qt for digiKam bundle.
#
# Copyright (c) 2015-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(PREFIX_ext_gmicqt "${EXTPREFIX}" )

set(GmicQt_Conf -DGMIC_QT_HOST=digikam
                -DENABLE_ASAN=OFF
                -DENABLE_FFTW3=OFF
                ${GLOBAL_PROFILE}
)

JoinListAsString("${GmicQt_Conf}" " " BASH_OPTIONS)

# Download the gmic sources
ExternalProject_Add(ext_gmic_base
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://github.com/dtschump/gmic.git

    SOURCE_DIR gmic

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

# Download and build gmic-qt
if (MINGW)

    ExternalProject_Add(ext_gmic_qt

        DEPENDS ext_gmic_base

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

else()

    ExternalProject_Add(ext_gmic_qt

        DEPENDS ext_gmic_base

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/cgilles/gmic-qt.git

        SOURCE_DIR gmic-qt
        INSTALL_DIR ${PREFIX_ext_gmicqt}

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_gmic} -DCMAKE_BUILD_TYPE=RelWithDebInfo ${GmicQt_Conf}

        UPDATE_COMMAND ""
    )

endif()
