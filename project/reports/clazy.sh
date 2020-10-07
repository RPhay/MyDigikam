#!/bin/bash

# Copyright (c) 2013-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Clang Clazy analyzer on whole digiKam source code.
# https://github.com/KDE/clazy
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

. ./common.sh

checksCPUCores

ORIG_WD="`pwd`"
REPORT_DIR="${ORIG_WD}/report.clazy"
WEBSITE_DIR="${ORIG_WD}/site"

# Get active git branches to create report description string
TITLE="digiKam-$(parseGitBranch)$(parseGitHash)"
echo "Clazy Scan Static Analyzer task name: $TITLE"

# Do not parse unwanted directories accordingly with Krazy configuration.
krazySkipConfig

IGNORE_DIRS=".*include.*|"

for DROP_ITEM in $KRAZY_FILTERS ; do
    IGNORE_DIRS+=".*$DROP_ITEM.*|"
done

# Remove last character
IGNORE_DIRS=${IGNORE_DIRS::-1}

export CLAZY_IGNORE_DIRS=$IGNORE_DIRS
export CLAZY_CHECKS="level1,no-fully-qualified-moc-types,no-overloaded-signal,no-qproperty-without-notify"

echo "IGNORE DIRECTORIES:     $CLAZY_IGNORE_DIRS"
echo "CHECKERS CONFIGURATION: $CLAZY_CHECKS"

# Clean up and prepare to scan.

rm -fr $REPORT_DIR
rm -fr $WEBSITE_DIR
mkdir -p $REPORT_DIR

cd ../..

rm -fr build.clazy
mkdir -p build.clazy
cd build.clazy

cmake -G "Unix Makefiles" . \
      -DCMAKE_BUILD_TYPE=debug \
      -DCMAKE_CXX_COMPILER=clazy \
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
      -DENABLE_DBUS=ON \
      -DENABLE_APPSTYLES=ON \
      -DENABLE_QWEBENGINE=ON \
      -Wno-dev \
      ..

make -j$CPU_CORES 2> ${REPORT_DIR}/trace.log

python3 ./clazy_visualizer.py $REPORT_DIR/trace.log

#rm -f $REPORT_DIR/trace.log
mv clazy.html $REPORT_DIR/index.html

cd $ORIG_WD

CLAZYBUILD_DIR=$(find ${REPORT_DIR} -maxdepth 1 -not -empty -not -name `basename ${REPORT_DIR}`)
echo "Clazy Report $TITLE to publish is located to $CLAZYBUILD_DIR"

# update www.digikam.org report section.
updateReportToWebsite "clang" $CLAZYBUILD_DIR $TITLE $(parseGitBranch)

cd $ORIG_DIR
