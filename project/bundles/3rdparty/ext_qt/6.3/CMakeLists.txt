# Script to build Qt for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_qt "${EXTPREFIX}")

IF(NOT ENABLE_QTWEBENGINE)
    SET(DROP_QTWEBENGINE_DEPS
        -skip qtwebengine                 # No need Chromium browser support (QtWebkit instead)
        -skip qtwebchannel                # QtWebChannel support ==> QWebEngine dependency
        -skip qtquickcontrols             # QtQuick support      ==> QWebEngine dependency
        -skip qtshadertools               # QWebEngine dependency
        )
ENDIF()

ExternalProject_Add(ext_qt

    URL file://${EXTERNALS_DOWNLOAD_DIR}/kde-qt6.tar

    PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt-appimage-support.patch &&
                  ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt-base-cmake.patch

    CONFIGURE_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/bootstrap-qt6.sh <SOURCE_DIR>/. &&
                      <SOURCE_DIR>/bootstrap-qt6.sh ${EXTPREFIX} ${DROP_QTWEBENGINE_DEPS}

    BUILD_COMMAND ninja

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)

ExternalProject_Add_step(ext_qt before_download

    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/kde-qt6.sh ${EXTERNALS_DOWNLOAD_DIR}
    DEPENDERS download
)
