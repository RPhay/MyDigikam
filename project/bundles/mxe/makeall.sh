#!/bin/bash

# Script to run all MXE based sub-scripts to build Windows installer.
# Possible option : "-f" to force operations without to ask confirmation to user.
#
# Copyright (c) 2013-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

########################################################################
# Function to upload host log files
HostUploadLogFiles()
{

if [[ $DK_UPLOAD = 1 ]] ; then

    echo -e "---------- Cleanup older host logs from files.kde.org repository \n"

    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR/build.logs/win64 <<< "rm build-mxe.full.log.gz"
    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR/build.logs/win64 <<< "rm build-extralibs.full.log.gz"

    echo -e "---------- Compress host log files \n"

    gzip -k $ORIG_WD/logs/build-mxe.full.log $ORIG_WD/logs/build-mxe.full.log.gz             || true
    gzip -k $ORIG_WD/logs/build-extralibs.full.log $ORIG_WD/logs/build-extralibs.full.log.gz || true

    echo -e "---------- Upload new host logs to files.kde.org repository \n"

    rsync -r -v --progress -e ssh $ORIG_WD/logs/build-mxe.full.log.gz $DK_UPLOADURL:$DK_UPLOADDIR/build.logs/win64       || true
    rsync -r -v --progress -e ssh $ORIG_WD/logs/build-extralibs.full.log.gz $DK_UPLOADURL:$DK_UPLOADDIR/build.logs/win64 || true

    echo -e "---------- Cleanup local bundle log file archives \n"

    rm -f $ORIG_WD/logs/build-mxe.full.log.gz       || true
    rm -f $ORIG_WD/logs/build-extralibs.full.log.gz || true

fi

}

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR
trap HostUploadLogFiles ERR exit

ORIG_WD="`pwd`"

. ./config.sh
. ./common.sh
StartScript

echo "This script will build from scratch the digiKam installer for Windows using MXE."
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

if [[ -d "`pwd`/build.win64" ]] ; then

    if [ "$1" != "-f" ] ; then

        read -p "Previous MXE build already exist and it will be removed. Do you want to continue ? [(c)ontinue/(s)top] " answer

        if echo "$answer" | grep -iq "^s" ; then

            echo "---------- Aborting..."
            exit;

        fi

    fi

    echo "---------- Removing existing MXE build"
    rm -fr `pwd`/build.win64

fi

echo "++++++++++++++++   Build 64 bits Installer   ++++++++++++++++++++++++++++++++++"
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

./01-build-mxe.sh
./02-build-extralibs.sh
./update.sh

echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

TerminateScript
