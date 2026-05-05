#!/bin/bash

# SPDX-FileCopyrightText: 2013-2026 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Clazy analyzer on whole digiKam source code.
# https://github.com/KDE/clazy
# Dependencies : Python BeautifulSoup and SoupSieve at run-time.
#
# If '--nowebupdate' is passed as argument, static analyzer results are not pushed online at
# https://files.kde.org/digikam/reports/ (default yes).
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

. ./common.sh

StartScript

# Analyzer configuration.
. ../../.clazy

# Check run-time dependencies

if [ ! -f /usr/bin/clazy ] ; then

    echo "Clazy static analyzer is not installed in /opt/clazy."
    echo "Please install Clazy from https://github.com/KDE/clazy"
    echo "Aborted..."
    exit -1


else

    echo "Check Clazy static analyzer passed..."

fi

checksCPUCores

ORIG_WD="`pwd`"
REPORT_DIR="report.clazy"

# Get active git branches to create report description string
TITLE="digiKam-$(parseGitBranch)$(parseGitHash)"
echo "Clazy Static Analyzer task name: $TITLE"

echo "IGNORE DIRS CONFIGURATION: $CLAZY_IGNORE_DIRS"
echo "CHECKERS CONFIGURATION:    $CLAZY_CHECKS"

# Clean up and prepare to scan.

rm -fr $ORIG_WD/$REPORT_DIR
mkdir -p $ORIG_WD/$REPORT_DIR

cd ../..

rm -fr build.clazy
mkdir -p build.clazy
cd build.clazy

#export PATH=$PATH:/opt/clazy/bin

if [[ -d /opt/qt6 ]] ; then

    export BUILD_WITH_QT6=1
    export Qt6_DIR=/opt/qt6
    QTPATHS="/opt/qt6/bin/qtpaths6"
    export CMAKE_BINARY=/opt/qt6/bin/cmake

else

    export BUILD_WITH_QT6=0
    QTPATHS="qtpaths"

fi


$CMAKE_BINARY -G "Unix Makefiles" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_COMPILER_LAUNCHER=clazy \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
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

 #     -DCMAKE_CXX_FLAGS="-I/usr/lib/llvm-21/lib/clang/21.1.8/include -isystem /usr/include/c++/15 -isystem /usr/include/$(uname -m)-linux-gnu/c++/15" \

make -j$CPU_CORES 2> ${ORIG_WD}/${REPORT_DIR}/trace.log

cd $ORIG_WD

python3 ./clazy_visualizer.py $ORIG_WD/$REPORT_DIR/trace.log

rm -f $ORIG_WD/$REPORT_DIR/trace.log
mv clazy.html $ORIG_WD/$REPORT_DIR/index.html

if [[ $1 != "--nowebupdate" ]] ; then

    cd $ORIG_WD
    updateOnlineReport "clazy" $REPORT_DIR $TITLE $(parseGitBranch)

fi

cd $ORIG_WD

rm -fr ../../build.clazy

TerminateScript
