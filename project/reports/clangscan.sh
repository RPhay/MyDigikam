#!/bin/bash

# SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Clang static analyzer on whole digiKam source code.
# https://clang-analyzer.llvm.org/
# Dependencies : clang static analyzer version >= 3.9.0.
#
# If '--webupdate' is passed as argument, static analyzer results are just created locally.
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

. ./common.sh

checksCPUCores

ORIG_WD="`pwd`"
REPORT_DIR="${ORIG_WD}/report.scan"
WEBSITE_DIR="${ORIG_WD}/site"

# Get active git branches to create report description string
TITLE="digiKam-$(parseGitBranch)$(parseGitHash)"
echo "Clang Scan Static Analyzer task name: $TITLE"

# Clean up and prepare to scan.

rm -fr $REPORT_DIR
rm -fr $WEBSITE_DIR

cd ../..

rm -fr build.scan
mkdir -p build.scan
cd build.scan

if [[ -d /opt/qt6 ]] ; then

    export BUILD_WITH_QT6=1
    export Qt6_DIR=/opt/qt6
    QTPATHS="/opt/qt6/bin/qtpaths6"
    export CMAKE_BINARY=/opt/qt6/bin/cmake

else

    export BUILD_WITH_QT6=0
    QTPATHS="qtpaths"
    export CMAKE_BINARY=cmake

fi

# Do not parse unwanted directories accordingly with Krazy configuration.
krazySkipConfig

IGNORE_DIRS=""

for DROP_ITEM in $KRAZY_FILTERS ; do
    IGNORE_DIRS+="--exclude ../../$DROP_ITEM/ "
done

scan-build $CMAKE_BINARY -G "Unix Makefiles" \
      -DCMAKE_BUILD_TYPE=debug \
      -DBUILD_WITH_QT6=$BUILD_WITH_QT6 \
      -DBUILD_TESTING=ON \
      -DDIGIKAMSC_CHECKOUT_PO=OFF \
      -DDIGIKAMSC_CHECKOUT_DOC=OFF \
      -DDIGIKAMSC_COMPILE_PO=OFF \
      -DDIGIKAMSC_COMPILE_DOC=OFF \
      -DENABLE_KFILEMETADATASUPPORT=ON \
      -DENABLE_AKONADICONTACTSUPPORT=ON \
      -DENABLE_MYSQLSUPPORT=ON \
      -DENABLE_INTERNALMYSQL=ON \
      -DENABLE_MEDIAPLAYER=ON \
      -DENABLE_QTMULTIMEDIA=ON \
      -DENABLE_DBUS=ON \
      -DENABLE_APPSTYLES=ON \
      -DENABLE_GEOLOCATION=ON \
      -DENABLE_QWEBENGINE=ON \
      -Wno-dev \
      ..

scan-build -o $REPORT_DIR \
           -no-failure-reports \
           -v \
           -k \
           --keep-empty \
           --html-title $TITLE \
           $IGNORE_DIRS \
           make -j$CPU_CORES

cd $ORIG_WD

SCANBUILD_DIR=$(find ${REPORT_DIR} -maxdepth 1 -not -empty -not -name `basename ${REPORT_DIR}`)
echo "Clang Report $TITLE is located to $SCANBUILD_DIR"

if [[ $1 == "--webupdate" ]] ; then

    # update www.digikam.org report section.
    updateReportToWebsite "clang" $SCANBUILD_DIR $TITLE $(parseGitBranch)

fi

cd $ORIG_DIR
