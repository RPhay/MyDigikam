#!/bin/bash

# Script to build a Qt 5.15 LST from KDE compilation patches repository.
# https://community.kde.org/Qt5PatchCollection
#
# Arguments : $1 : download directory.
#             $2 : QtWebEngine version.
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set -e
set -C

if [ "$1" == "" ]; then
    DOWNLOAD_DIR=$PWD
else
    DOWNLOAD_DIR=$1
fi

echo "Download directory: $DOWNLOAD_DIR"

# --- Create a new fresh checkout of code if do not exists yet, or just update.

if [[ ! -d $DOWNLOAD_DIR/kde-5.15-LTS ]] ; then

    echo "Checkout Git module sub-directories from kde/5.15 LTS repository"

    cd $DOWNLOAD_DIR
    git clone --progress --verbose --branch kde/5.15 https://invent.kde.org/qt/qt/qt5.git kde-5.15-LTS
    cd kde-5.15-LTS

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

    WEBENGINE_LTS=v$QTWEBENGINE_VERSION-lts

    echo "Branching QtWebEngine to LTS version $WEBENGINE_LTS..."
    cd qtwebengine
    git checkout $WEBENGINE_LTS || true
    cd ..

else

    echo "Update Git module sub-directories from kde/5.15 LTS repository"

    cd $DOWNLOAD_DIR/kde-5.15-LTS

    QT_SUBDIRS=$(ls -F | grep / | grep qt)

    git submodule update --recursive --progress

fi

# --- List git revisions for all sub-modules

QT5_GITREV_LST=$DOWNLOAD_DIR/qt_manifest.txt

echo "List git sub-module revisions"

cd $DOWNLOAD_DIR/kde-5.15-LTS

rm -f $QT5_GITREV_LST
currentDate=`date +"%Y-%m-%d"`
echo "+QT Snapshot $currentDate" > $QT5_GITREV_LST

for SUBDIR in $QT_SUBDIRS ; do

    cd $SUBDIR
    echo "$(basename "$SUBDIR"):$(git rev-parse HEAD)" >> $QT5_GITREV_LST
    cd ..

done

cat $QT5_GITREV_LST

# --- Create a non compressed archive for cmake download stage.

# NOTE: do not remove .git sub directories as these are used to compile source code.

echo "Archive local repository"

rm -f $DOWNLOAD_DIR/kde-5.15-LTS.tar
tar -cf $DOWNLOAD_DIR/kde-5.15-LTS.tar -C $DOWNLOAD_DIR/kde-5.15-LTS .
