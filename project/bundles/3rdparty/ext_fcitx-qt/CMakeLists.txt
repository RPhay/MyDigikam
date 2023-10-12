# Script to build Fcitx-qt for digiKam bundle.
#
# SPDX-FileCopyrightText: 2015-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(EXTPREFIX_fcitx-qt "${EXTPREFIX}" )

if(${ENABLE_QTVERSION} MATCHES "5")

    message(STATUS "Compiling fcitx-qt for Qt5")

    ExternalProject_Add(ext_fcitx-qt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/fcitx/fcitx-qt5.git
        GIT_TAG 1.2.7

        CONFIGURE_COMMAND cd <SOURCE_DIR> &&
                          $(CMAKE_COMMAND) .
                             -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_fcitx-qt}
                             -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                             -DENABLE_LIBRARY=OFF
                             -DENABLE_QT5=ON
                             -DENABLE_QT6=OFF
                             ${GLOBAL_PROFILE}

        UPDATE_COMMAND ""
        BUILD_IN_SOURCE 0
        ALWAYS 0
    )

else()

    message(STATUS "Compiling fcitx-qt for Qt6")

    ExternalProject_Add(ext_fcitx-qt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/fcitx/fcitx-qt5.git
        GIT_TAG 1.2.7

        CONFIGURE_COMMAND cd <SOURCE_DIR> &&
                          $(CMAKE_COMMAND) .
                             -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                             -DENABLE_LIBRARY=OFF
                             -DENABLE_QT5=OFF
                             -DENABLE_QT6=ON
                             ${GLOBAL_PROFILE}

        UPDATE_COMMAND ""
        BUILD_IN_SOURCE 0
        ALWAYS 0
    )

endif()
