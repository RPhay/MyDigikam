#!/bin/bash

# Script to build a Qt 6 from KDE repository.
#
# Arguments : $1 : download directory.
#             $2 : Qt6 version
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

QT6_VERSION="v$2"

echo "Download directory: $DOWNLOAD_DIR"
echo "Qt6 version: $QT6_VERSION"

# --- Create a new fresh checkout of code if do not exists yet, or just update.

if [[ ! -d $DOWNLOAD_DIR/kde-qt6 ]] ; then

    echo "Checkout Git module sub-directories from kde/qt6 repository"

    cd $DOWNLOAD_DIR
    git clone --progress --verbose --branch $QT6_VERSION https://invent.kde.org/qt/qt/qt5.git kde-qt6
    cd kde-qt6

    git submodule update --init --recursive --progress

    # Switch sub-modules to qt6 branches

    QT_SUBDIRS=$(ls -F | grep / | grep qt)

    echo "Git module sub-directories to switch to $QT6_VERSION branch: $QT_SUBDIRS"

    for SUBDIR in $QT_SUBDIRS ; do

        echo "Branching $SUBDIR to $QT6_VERSION..."
        cd $SUBDIR
        git checkout $QT6_VERSION || true
        cd ..

    done

else

    echo "Update Git module sub-directories from kde/qt6 repository"

    cd $DOWNLOAD_DIR/kde-qt6

    QT_SUBDIRS=$(ls -F | grep / | grep qt)

    git submodule update --recursive --progress

fi

# --- List git revisions for all sub-modules

QT6_GITREV_LST=$DOWNLOAD_DIR/qt_manifest.txt

echo "List git sub-module revisions"

cd $DOWNLOAD_DIR/kde-qt6

rm -f $QT6_GITREV_LST
currentDate=`date +"%Y-%m-%d"`
echo "+QT Snapshot $currentDate" > $QT6_GITREV_LST

for SUBDIR in $QT_SUBDIRS ; do

    cd $SUBDIR
    echo "$(basename "$SUBDIR"):$(git rev-parse HEAD)" >> $QT6_GITREV_LST
    cd ..

done

cat $QT6_GITREV_LST

# --- Create a non compressed archive for cmake download stage.

# NOTE: do not remove .git sub directories as these are used to compile source code.

echo "Archive local repository"

rm -f $DOWNLOAD_DIR/kde-qt6.tar
tar -cf $DOWNLOAD_DIR/kde-qt6.tar -C $DOWNLOAD_DIR/kde-qt6 .
