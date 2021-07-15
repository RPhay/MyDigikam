#!/bin/bash

# Script to build a Qt 5.15 LST from KDE compilation patches repository.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set -e
set -C

#git clone https://invent.kde.org/qt/qt/qt5.git kde-5.15-LTS
#cd kde-5.15-LTS
#git checkout kde/5.15

git submodule update --init --progress

rm -rf                  \
    qt3d                \
    qtactiveqt          \
    qtandroidextras     \
    qtcanvas3d          \
    qtcharts            \
    qtconnectivity      \
    qtdatavis3d         \
    qtdoc               \
    qtdocgallery        \
    qtfeedback          \
    qtgamepad           \
    qtgraphicaleffects  \
    qtlocation          \
    qtlottie            \
    qtmacextras         \
    qtmultimedia        \
    qtnetworkauth       \
    qtpim               \
    qtpurchasing        \
    qtqa                \
    qtquick3d           \
    qtquickcontrols2    \
    qtquicktimeline     \
    qtremoteobjects     \
    qtrepotools         \
    qtscript            \
    qtscxml             \
    qtsensors           \
    qtserialbus         \
    qtserialport        \
    qtspeech            \
    qtsystems           \
    qtvirtualkeyboard   \
    qtwebglplugin       \
    qtwebsockets        \
    qtwebview           \
    qtwinextras

QT_SUBDIRS=$(ls -F | grep / | grep qt)

echo "Git module sub-directories to switch to kde/5.15 branch: $QT_SUBDIRS"

for SUBDIR in $QT_SUBDIRS ; do

    echo "Branching $SUBDIR to kde/5.15..."
    cd $SUBDIR
    git checkout kde/5.15
    cd ..

done

