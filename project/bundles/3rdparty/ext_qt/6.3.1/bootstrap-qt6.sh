#!/bin/bash

# Script to build a Qt 6 from KDE repository.
#
# Arguments : $1 : install prefix.
#             $2 : QtWebEngine dependencies to drop.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set -e
set -C

# Note: code must be compiled to source dir else qtwebengine will not be compiled due to qtquick configuration failure.
rm -fr CMakeFiles
rm -f CMakeCache.txt

EXT_PREFIX=$1
DROP_QTWEBENGINE_DEPS=$2

# NOTE: even if OpenSSL libs paths configuration is passed heres, an hardcoded patch is necessary else linking fail in openssl plugin.
OPENSSL_LIBS='-L$EXT_PREFIX/local/include/openssl/ -l$EXT_PREFIX/local/lib/libssl.a -l$EXT_PREFIX/local/lib/libcrypto.a -lpthread -ldl'

./configure       -cmake-generator Ninja                `#  Qt6 use Ninja build system by default.` \
                  -prefix $EXT_PREFIX                   `#  Install path for the install stage.` \
                  -release                              `#  Release compiled version.` \
                  -opensource                           `#  Open Source compiled version.` \
                  -confirm-license                      `#  Valid directly the license agreement.` \
                  -sql-sqlite                           `#  Compile Sqlite SQL plugin.` \
                  -sql-mysql                            `#  Compile Mysql SQL plugin.` \
                  -fontconfig                           `#  .` \
                  -system-freetype                      `#  Use system font rendering lib https://doc.qt.io/qt-5/qtgui-attribution-freetype.html.` \
                  -openssl-linked                       `#  hard link to static version of ssl libraries.` \
                  -nomake tests                         `#  Do not build test codes.` \
                  -nomake examples                      `#  Do not build basis example codes.` \
                  -no-qml-debug                         `#  .` \
                  -no-mtdev                             `#  .` \
                  -no-journald                          `#  .` \
                  -no-syslog                            `#  .` \
                  -no-tslib                             `#  .` \
                  -no-directfb                          `#  .` \
                  -no-linuxfb                           `#  .` \
                  -no-libproxy                          `#  .` \
                  -no-pch                               `#  .` \
                  -qt-zlib                              `#  .` \
                  -qt-pcre                              `#  .` \
                  -qt-harfbuzz                          `#  .` \
                  -xcb                                  `#  .` \
                  -skip qt3d                            `#  3D core.` \
                  -skip qtactiveqt                      `#  No need ActiveX support.` \
                  -skip qtcanvas3d                      `#  New to Qt6.` \
                  -skip qtcharts                        `#  No need data models charts support.` \
                  -skip qtcoap                          `#  New to Qt6.` \
                  -skip qtconnectivity                  `#  For embedded devices only.` \
                  -skip qtdatavis3d                     `#  no need 3D data visualizations support.` \
                  -skip qtdoc                           `#  No need documentation.` \
                  -skip qtfeedback                      `#  New to Qt6.` \
                  -skip qtgamepad                       `#  No need gamepad hardware support..` \
                  -skip qtgraphicaleffects              `#  No need Advanced graphical effects in GUI.` \
                  -skip qtlanguageserver                `#  New to Qt6.` \
                  -skip qtlocation                      `#  No need geolocation.` \
                  -skip qtlottie                        `#  No need Adobe QtQuick After Effect animations integration.` \
                  -skip qtmqtt                          `#  No need Mqtt support.` \
                  -skip qtmultimedia                    `#  No need multimedia support (replaced by QtAV+ffmpeg).` \
                  -skip qtopcua                         `#  New to Qt6.` \
                  -skip qtpim                           `#  New to Qt6.` \
                  -skip qtpositioning                   `#  New to Qt6.` \
                  -skip qtqa                            `#  New to Qt6.` \
                  -skip qtpurchasing                    `#  No need in-app purchase of products support.` \
                  -skip qtquick3d                       `#  New to Qt6.` \
                  -skip qtquicktimeline                 `#  New to Qt6.` \
                  -skip qtremoteobjects                 `#  No need sharing QObject properties between processes support.` \
                  -skip qtrepotools                     `#  New to Qt6.` \
                  -skip qtscxml                         `#  No need SCXML state machines support.` \
                  -skip qtsensors                       `#  For embedded devices only.` \
                  -skip qtserialbus                     `#  No need serial bus support.` \
                  -skip qtserialport                    `#  No need serial port support.` \
                  -skip qtspeech                        `#  No need speech synthesis support.` \
                  -skip qtsystems                       `#  New to Qt6.` \
                  -skip qtvirtualkeyboard               `#  No need virtual keyboard support.` \
                  -skip qtwebglplugin                   `#  No need browser OpenGL extension support.` \
                  -skip qtwebsockets                    `#  No need websocket support.` \
                  -skip qtwebview                       `#  QML extension for QWebEngine.` \
                  $DROP_QTWEBENGINE_DEPS                `#  Extra QtWebEngine dependencies to drop if this module is disabled.` \

