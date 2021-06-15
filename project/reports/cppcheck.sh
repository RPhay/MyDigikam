#!/bin/bash

# Copyright (c) 2013-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run CppCheck static analyzer on whole digiKam source code.
# http://cppcheck.sourceforge.net/
# Dependencies : Python::pygments module to export report as HTML.
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

. ./common.sh

checksCPUCores

ORIG_WD="`pwd`"
REPORT_DIR="${ORIG_WD}/report.cppcheck"
WEBSITE_DIR="${ORIG_WD}/site"

# Get active git branches to create report description string
TITLE="digiKam-$(parseGitBranch)$(parseGitHash)"
echo "CppCheck Static Analyzer task name: $TITLE"

rm -fr $REPORT_DIR
rm -fr $WEBSITE_DIR

# Do not parse unwanted directories accordingly with Krazy configuration.
krazySkipConfig

IGNORE_DIRS=""

for DROP_ITEM in $KRAZY_FILTERS ; do
    IGNORE_DIRS+="-i../../$DROP_ITEM/ "
done

# List sub-dirs with headers to append as cppcheck includes pathes
HDIRS=$(find ../../core -name '*.h' -printf '%h\n' | sort -u)

for INCLUDE_PATH in $HDIRS ; do
    INCLUDE_DIRS+="-I $INCLUDE_PATH/ "
done

cppcheck -j$CPU_CORES \
         -DQ_OS_LINUX \
         --verbose \
         --std=c++14 \
         --library=qt.cfg \
         --inline-suppr \
         --xml-version=2 \
         --platform=unix64 \
         --enable=all \
         --report-progress \
         --suppress=*:*cimg*.h \
         --suppress=*:*libraw*.h \
         --suppress=*:*libde265*.h \
         --suppress=*:*libheif*.h \
         --suppress=*:*libpgf*.h \
         --suppress=*:*upnpsdk*.h \
         --suppress=*:*yfauth*.h \
         --suppress=*:*o2*.h \
         --suppress=*:*libjpeg*.h \
         --suppress=*:*dng_sdk*.h \
         --suppress=*:*xmp_sdk*.h \
         --suppress=variableScope \
         --suppress=purgedConfiguration \
         --suppress=toomanyconfigs \
         --suppress=unreadVariable \
         --suppress=unusedVariable \
         --suppress=unusedStructMember \
         --suppress=unknownMacro \
         --suppress=class_X_Y \
         --suppress=ConfigurationNotChecked \
         --suppress=unmatchedSuppression \
         --output-file=report.cppcheck.xml \
         $IGNORE_DIRS \
         $INCLUDE_DIRS \
         ../../core

cppcheck-htmlreport --file=report.cppcheck.xml \
                    --report-dir=$REPORT_DIR \
                    --source-dir=. \
                    --title=$TITLE

# update www.digikam.org report section.
updateReportToWebsite "cppcheck" $REPORT_DIR $TITLE $(parseGitBranch)

cd $ORIG_DIR

