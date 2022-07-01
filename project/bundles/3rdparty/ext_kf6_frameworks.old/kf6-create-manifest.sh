#!/bin/bash

# Script to create KF6 framework manifest.
#
# Arguments : $1 : download directory.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set -e
set -C

if [ "$1" == "" ]; then
    DOWNLOAD_DIR=$PWD
else
    DOWNLOAD_DIR=$1
fi

echo "Download directory: $DOWNLOAD_DIR"

# --- List git revisions for all sub-modules

KF6_GITREV_LST=$DOWNLOAD_DIR/kf6_manifest.txt

echo "List git sub-module revisions"

cd $DOWNLOAD_DIR
KF6_SUBDIRS=$(ls -F | grep / | grep -v log | grep -v sysadmin)

rm -f $KF6_GITREV_LST
currentDate=`date +"%Y-%m-%d"`
echo "+KF6 Snapshot $currentDate" > $KF6_GITREV_LST

for SUBDIR in $KF6_SUBDIRS ; do

    cd $SUBDIR
    echo "$(basename "$SUBDIR"):$(git rev-parse HEAD)" >> $KF6_GITREV_LST
    cd ..

done

cat $KF6_GITREV_LST
