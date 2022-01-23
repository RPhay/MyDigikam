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
        )
ENDIF()

ExternalProject_Add(ext_qt

    URL file://${EXTERNALS_DOWNLOAD_DIR}/kde-qt6.tar

    PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt-appimage-support.patch
               && ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt-appimage-openssl-linked.patch
               && ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt-icu-hack.patch

    CONFIGURE_COMMAND <SOURCE_DIR>/configure
                  -prefix ${EXTPREFIX_qt}
                  -verbose
                  -release
                  -opensource
                  -confirm-license
                  -sql-sqlite                       # Compile Sqlite SQL plugin
                  -sql-mysql                        # Compile Mysql SQL plugin
#                  -iconv                            # International string conversion
                  -fontconfig
                  -system-freetype                  # Use system font rendering lib https://doc.qt.io/qt-5/qtgui-attribution-freetype.html
                  -openssl-linked                   # hard link to static version of ssl libraries
                  -nomake tests                     # Do not build test codes
                  -nomake examples                  # Do not build basis example codes
                  -no-compile-examples              # Do not build extra example codes
                  -no-qml-debug
                  -no-mtdev
                  -no-journald
                  -no-syslog
                  -no-tslib
                  -no-directfb
                  -no-linuxfb
                  -no-libproxy
                  -no-pch
                  -qt-zlib
                  -qt-pcre
                  -qt-harfbuzz
                  -xcb
                  -skip qt3d                        # 3D core
                  -skip qtactiveqt                  # No need ActiveX support
                  -skip qtcanvas3d                  # New to Qt6
                  -skip qtcharts                    # No need data models charts support
                  -skip qtcoap                      # New to Qt6
                  -skip qtconnectivity              # For embedded devices only
                  -skip qtdatavis3d                 # no need 3D data visualizations support
                  -skip qtdoc                       # No need documentation
                  -skip qtfeedback                  # New to Qt6
                  -skip qtgamepad                   # No need gamepad hardware support.
                  -skip qtgraphicaleffects          # No need Advanced graphical effects in GUI
                  -skip qtlanguageserver            # New to Qt6
                  -skip qtlocation                  # No need geolocation
                  -skip qtlottie                    # No need Adobe QtQuick After Effect animations integration
                  -skip qtmqtt                      # New to Qt6
                  -skip qtmultimedia                # No need multimedia support (replaced by QtAV+ffmpeg)
                  -skip qtnetworkauth               # No need network authentication support.
                  -skip qtopcua                     # New to Qt6
                  -skip qtpim                       # New to Qt6
                  -skip qtpositioning               # New to Qt6
                  -skip qtqa                        # New to Qt6
                  -skip qtpurchasing                # No need in-app purchase of products support
                  -skip qtquick3d                   # New to Qt6
                  -skip qtquicktimeline             # New to Qt6
                  -skip qtremoteobjects             # No need sharing QObject properties between processes support
                  -skip qtrepotools                 # New to Qt6
                  -skip qtscxml                     # No need SCXML state machines support
                  -skip qtsensors                   # For embedded devices only
                  -skip qtserialbus                 # No need serial bus support
                  -skip qtserialport                # No need serial port support
                  -skip qtshadertools               # New to Qt6
                  -skip qtspeech                    # No need speech synthesis support
                  -skip qtsystems                   # New to Qt6
                  -skip qttools                     # New to Qt6
                  -skip qtvirtualkeyboard           # No need virtual keyboard support
                  -skip qtwebglplugin               # No need browser OpenGL extension support
                  -skip qtwebsockets                # No need websocket support
                  -skip qtwebview                   # QML extension for QWebEngine

                  ${DROP_QTWEBENGINE_DEPS}

    # Example of code to install only one Qt module for hacking purpose (aka qtbase here)
    #BUILD_COMMAND cd <SOURCE_DIR> && $(MAKE) module-qtbase
    #INSTALL_COMMAND cd <SOURCE_DIR> && $(MAKE) module-qtbase-install_subtargets

    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    ALWAYS 0
)

ExternalProject_Add_step(ext_qt before_download

    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/kde-qt6.sh ${EXTERNALS_DOWNLOAD_DIR}
    DEPENDERS download
)
