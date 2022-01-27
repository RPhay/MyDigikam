#!/bin/bash

# Script to build a Qt 6 from KDE repository.
#
# Arguments : $1 : install prefix.
#             $2 : QtWebEngine dependencies to drop.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set -e
set -C

# Note: code must be compiled to source dir else qtwebengine will not be compiled due to qtquick configuration failure.
rm -fr CMakeFiles
rm -f CMakeCache.txt

EXT_PREFIX=$1
DROP_QTWEBENGINE_DEPS=$2

OPENSSL_LIBS='-L$EXT_PREFIX/local/include/openssl/ -l$EXT_PREFIX/local/lib/libssl.a -l$EXT_PREFIX/local/lib/libcrypto.a -lpthread -ldl' \

./configure       -cmake-generator Ninja\
                  -prefix $EXT_PREFIX \
                  -release \
                  -opensource \
                  -confirm-license \
                  -sql-sqlite \
                  -sql-mysql \
                  -fontconfig \
                  -system-freetype \
                  -openssl-linked \
                  -nomake tests \
                  -nomake examples \
                  -no-qml-debug \
                  -no-mtdev \
                  -no-journald \
                  -no-syslog \
                  -no-tslib \
                  -no-directfb \
                  -no-linuxfb \
                  -no-libproxy \
                  -no-pch \
                  -qt-zlib \
                  -qt-pcre \
                  -qt-harfbuzz \
                  -xcb \
                  -skip qt3d \
                  -skip qtactiveqt                   \
                  -skip qtcanvas3d                   \
                  -skip qtcharts                     \
                  -skip qtcoap                       \
                  -skip qtconnectivity               \
                  -skip qtdatavis3d                  \
                  -skip qtdoc                        \
                  -skip qtfeedback                   \
                  -skip qtgamepad                    \
                  -skip qtgraphicaleffects           \
                  -skip qtlanguageserver             \
                  -skip qtlocation                   \
                  -skip qtlottie                     \
                  -skip qtmqtt                       \
                  -skip qtmultimedia                 \
                  -skip qtnetworkauth                \
                  -skip qtopcua                      \
                  -skip qtpim                        \
                  -skip qtpositioning                \
                  -skip qtqa                         \
                  -skip qtpurchasing                 \
                  -skip qtquick3d                    \
                  -skip qtquicktimeline              \
                  -skip qtrepotools                  \
                  -skip qtremoteobjects              \
                  -skip qtscxml                      \
                  -skip qtsensors                    \
                  -skip qtserialbus                  \
                  -skip qtserialport                 \
                  -skip qtspeech                     \
                  -skip qtsystems                    \
                  -skip qtvirtualkeyboard            \
                  -skip qtwebglplugin                \
                  -skip qtwebsockets                 \
                  -skip qtwebview                    \
                  $DROP_QTWEBENGINE_DEPS

