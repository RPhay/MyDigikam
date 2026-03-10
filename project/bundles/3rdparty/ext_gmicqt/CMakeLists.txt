# Script to build GMic-Qt plugin for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2026 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(PREFIX_ext_gmicqt "${EXTPREFIX}")

set(GmicQt_Conf -DGMIC_QT_HOST=digikam
                -DENABLE_ASAN=OFF
                -DENABLE_FFTW3=ON
                -DENABLE_SYSTEM_GMIC=OFF
                -DENABLE_CURL=OFF
                -DBUILD_TESTING=OFF
                ${GLOBAL_PROFILE}
)

if(${ENABLE_QTVERSION} MATCHES "6")

    set(GmicQt_Conf ${GmicQt_Conf} -DBUILD_WITH_QT6=ON)

else()

    set(GmicQt_Conf ${GmicQt_Conf} -DBUILD_WITH_QT6=OFF)

endif()

JoinListAsString("${GmicQt_Conf}" " " BASH_OPTIONS)

# Download and build gmic-qt

if(MSVC)

    ExternalProject_Add(ext_gmic_qt

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/cgilles/digikam-gmic-qt.git

        CONFIGURE_COMMAND bash <SOURCE_DIR>/bootstrap.vcpkg ${PREFIX_ext_gmicqt} RelWithDebInfo ${GmicQt_Conf}

        BUILD_COMMAND cd <SOURCE_DIR>/build && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build && $(MAKE) install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

else() # Linux & macOS

    ExternalProject_Add(ext_gmic_qt

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/cgilles/digikam-gmic-qt.git

        SOURCE_DIR gmic-qt
        INSTALL_DIR ${PREFIX_ext_gmicqt}

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_gmic}
                   -DCMAKE_BUILD_TYPE=RelWithDebInfo
                   ${GmicQt_Conf}

        UPDATE_COMMAND ""
    )

endif()

