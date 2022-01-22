#!/bin/bash

# Script to build a Qt 5.15 LST from KDE compilation patches repository.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set -e
set -C

# These commands are processed previously by CMake.
#git clone --progress --verbose --branch kde/5.15 --single-branch https://invent.kde.org/qt/qt/qt5.git kde-5.15-LTS
#cd kde-5.15-LTS

git submodule update --init --recursive --progress

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
    git checkout kde/5.15 || true
    cd ..

done

# QtWebEngine is pulished as LTS officially in open source, so we can checkout the stable tag as well.

WEBENGINE_LTS=v5.15.8-lts

echo "Branching QtWebEngine to LTS version $WEBENGINE_LTS..."
cd qtwebengine
git checkout $WEBENGINE_LTS || true
cd ..

# List git revisions for all sub-modules

echo "List git sub-module revisions"

rm -f qt5_lts_gitrev.h
echo "#pragma once"                  > qt5_lts_gitrev.h
echo "#define QT5_LTS_GITREV \" \\" >> qt5_lts_gitrev.h

for SUBDIR in $QT_SUBDIRS ; do

    cd $SUBDIR
    echo "$(basename "$SUBDIR"):$(git rev-parse HEAD) \\" >> ../qt5_lts_gitrev.h
    cd ..

done

echo "\""                           >> qt5_lts_gitrev.h

cat qt5_lts_gitrev.h

# Remove .git sub directories for archiving purpose.
#
#GIT_SUBDIRS=$(find  . -name '.git')
#
#echo "Git internal sub-directories to remove: $GIT_SUBDIRS"
#
#for SUBDIR in $GIT_SUBDIRS ; do
#
#    echo "Removing $SUBDIR..."
#    rm -fr $SUBDIR
#
#done
