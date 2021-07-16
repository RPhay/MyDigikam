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

#git clone --progress --verbose --branch kde/5.15 --single-branch https://invent.kde.org/qt/qt/qt5.git kde-5.15-LTS
#cd kde-5.15-LTS

git submodule update --init --progress

# Remove Qt6 sub-modules

rm -rf                  \
    qtcanvas3d          \
    qtdocgallery        \
    qtfeedback          \
    qtpim               \
    qtqa                \
    qtrepotools         \
    qtsystems

# Switch sub-modules to kde/5.15 branches

QT_SUBDIRS=$(ls -F | grep / | grep qt)

echo "Git module sub-directories to switch to kde/5.15 branch: $QT_SUBDIRS"

for SUBDIR in $QT_SUBDIRS ; do

    echo "Branching $SUBDIR to kde/5.15..."
    cd $SUBDIR
    git checkout kde/5.15
    cd ..

done

