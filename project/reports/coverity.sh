#!/bin/bash

# Copyright (c) 2013-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Coverity Scan static analyzer on whole digiKam source code.
# https://scan.coverity.com/
#
# Before to run this script you must set these Shell variable:
# $DKCoverityToken with token of digiKam project given by Coverity SCAN.
# $DKCoverityEmail with email adress to send SCAN result.
#
# See this url to see how to prepare your computer with Coverity SCAN tool:
# https://scan.coverity.com/projects/digikam/builds/new
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

cd ../..
if [ ] ; then
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
fi
# Get active git branches to create report description string
./gits branch | sed -e "s/*/#/g" | sed -e "s/On:/#On:/g" | grep "#" | sed -e "s/#On:/On:/g" | sed -e "s/#/BRANCH:/g" > ./build/git_branches.txt
desc=$(<build/git_branches.txt)

cd ./build

CPU_CORES=$(grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)
echo "CPU cores detected to compile : $CPU_CORES."

#cov-build --dir cov-int --tmpdir ~/tmp make -j$CPU_CORES
#tar czvf myproject.tgz cov-int

echo "-- SCAN Import description --"
echo $desc
echo "Analysis archive to upload:"
file myproject.tgz
du -H myproject.tgz
echo "-----------------------------"

echo "Coverity Scan tarball 'myproject.tgz' uploading in progress..."

# To be sure that resolv domain is in cache
nslookup scan5.coverity.com

# To mesure uploading time
SECONDS=0

curl https://scan.coverity.com/builds?project=digiKam \
     --insecure \
     --progress-bar \
     --verbose \
     --form token=$DKCoverityToken \
     --form email=$DKCoverityEmail \
     --form file=@myproject.tgz \
     --form version=git \
     --form description="$desc" \
     | tee -a "${LOG_FILE}" ; test ${PIPESTATUS[0]} -eq 0

echo "Done. Coverity Scan tarball 'myproject.tgz' is uploaded."
echo "That took approximately $SECONDS seconds to upload."
echo "File will be post processed for analyse. A mail notification will be send to digikam-bugs-null@kde.org."
