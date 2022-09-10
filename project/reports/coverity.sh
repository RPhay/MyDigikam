#!/bin/bash

# SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Coverity Scan static analyzer on whole digiKam source code.
# https://scan.coverity.com/
#
# Before to run this script you must:
#
# - Install Coverity Scan to /opt/ from https://scan.coverity.com/download?tab=cxx
# - Export binary path /opt/_coverity_version_/bin to PATH variable
# - Export $DKCoverityToken Shell variable with token of digiKam project given by Coverity SCAN web interface.
#
# Example in .bashrc:
#
# export DKCoverityToken=xxxxxxx
# export PATH=$PATH:/opt/cov-analysis-linux64-2020.09/bin
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

ORIG_WD="`pwd`"

# Check run-time dependencies

if ! which cov-build ; then

    echo "Coverity SCAN Toolkit is not installed!"
    echo "See https://scan.coverity.com/download?tab=cxx for details."
    exit -1

fi

# Check if Coverity token is set.

if [ ! compgen -e -X "!$DKCoverityToken" ] ; then

    echo "Coverity SCAN token variable is not set!"
    exit -1

fi

echo "Check Coverity SCAN Toolkit passed..."

cd $ORIG_WD/../..

# Manage build sub-dir

if [ -d "build" ]; then

    rm -rfv ./build

fi

if [[ "$OSTYPE" == "linux-gnu" ]]; then

    ./bootstrap.linux

elif [[ "$OSTYPE" == "darwin"* ]]; then

    ./bootstrap.macports

else

    echo "Unsupported platform..."
    exit -1

fi

# Get active git branches to create report description string
./gits branch | sed -e "s/*/#/g" | sed -e "s/On:/#On:/g" | grep "#" | sed -e "s/#On:/On:/g" | sed -e "s/#/BRANCH:/g" > ./build/git_branches.txt
desc=$(<build/git_branches.txt)

cd $ORIG_WD/../../build

CPU_CORES=$(grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)
echo "CPU cores detected to compile : $CPU_CORES."

cov-build --dir cov-int --tmpdir ~/tmp make -j$CPU_CORES
tar czvf myproject.tgz cov-int

cd $ORIG_WD/../../build

echo "-- SCAN Import description --"
echo $desc
echo "Analysis archive to upload:"
file myproject.tgz
du -H myproject.tgz
echo "-----------------------------"

echo "Coverity Scan tarball 'myproject.tgz' uploading in progress..."

# To be sure that resolve domain is in cache
nslookup scan.coverity.com

# To measure uploading time
SECONDS=0

curl https://scan.coverity.com/builds?project=digiKam \
     --progress-bar \
     --verbose \
     --form token=$DKCoverityToken \
     --form email=$digikam-devel@kde.org \
     --form file=@myproject.tgz \
     --form version=git \
     --form description="$desc"

echo "Done. Coverity Scan tarball 'myproject.tgz' is uploaded."
echo "That took approximately $SECONDS seconds to upload."
echo "File will be post processed for analyse. A mail notification will be send to digikam-devel@kde.org."
