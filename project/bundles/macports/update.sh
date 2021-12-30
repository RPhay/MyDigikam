#!/bin/bash

# Script to update digiKam MacOS package.
#
# Copyright (c) 2013-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

########################################################################
# Function to upload bundle log files
BundleUploadLogFiles()
{

if [[ $DK_UPLOAD = 1 ]] ; then

    echo -e "---------- Cleanup older bundle logs from files.kde.org repository \n"

    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR/build.logs/macos <<< "rm build-digikam.full.log.gz"
    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR/build.logs/macos <<< "rm build-installer.full.log.gz"

    echo -e "---------- Compress bundle log files \n"

    gzip -k $ORIG_WD/logs/build-digikam.full.log $ORIG_WD/logs/build-digikam.full.log.gz     || true
    gzip -k $ORIG_WD/logs/build-installer.full.log $ORIG_WD/logs/build-installer.full.log.gz || true

    echo -e "---------- Upload new bundle logs to files.kde.org repository \n"

    rsync -r -v --progress -e ssh $ORIG_WD/logs/build-digikam.full.log.gz $DK_UPLOADURL:$DK_UPLOADDIR/build.logs/macos   || true
    rsync -r -v --progress -e ssh $ORIG_WD/logs/build-installer.full.log.gz $DK_UPLOADURL:$DK_UPLOADDIR/build.logs/macos || true

    echo -e "---------- Cleanup local bundle log file archives \n"

    rm -f $ORIG_WD/logs/build-digikam.full.log.gz   || true
    rm -f $ORIG_WD/logs/build-installer.full.log.gz || true

fi

}

########################################################################

# Ask to run as root
(( EUID != 0 )) && exec sudo -- "$0" "$@"

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR
trap BundleUploadLogFiles ERR exit

. ./config.sh
. ./common.sh
StartScript
ChecksRunAsRoot
ChecksXCodeCLI
ChecksCPUCores

ORIG_WD="`pwd`"

echo "++++++++++++++++++++ Update MacOS Installer +++++++++++++++++++++++++++++++++++"
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

./03-build-digikam.sh

sed -e "s/DK_DEBUG=1/DK_DEBUG=0/g"           ./config.sh > ./tmp.sh ; mv -f ./tmp.sh ./config.sh

./04-build-installer.sh

sed -e "s/DK_DEBUG=0/DK_DEBUG=1/g"           ./config.sh > ./tmp.sh ; mv -f ./tmp.sh ./config.sh

./04-build-installer.sh

sed -e "s/DK_DEBUG=1/DK_DEBUG=0/g"           ./config.sh > ./tmp.sh ; mv -f ./tmp.sh ./config.sh

echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

TerminateScript
