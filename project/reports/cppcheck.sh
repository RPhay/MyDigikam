#!/bin/bash

# SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run CppCheck static analyzer on whole digiKam source code.
# http://cppcheck.sourceforge.net/
# Dependencies : Python::pygments module to export report as HTML.
#
# If '--webupdate' is passed as argument, static analyzer results are pushed on the web site.
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

# List sub-dirs with headers to append as cppcheck includes paths
HDIRS=$(find ../../core -name '*.h' -printf '%h\n' | sort -u)

for INCLUDE_PATH in $HDIRS ; do
    INCLUDE_DIRS+="-I $INCLUDE_PATH/ "
done

cppcheck -j$CPU_CORES \
         -DQ_OS_LINUX \
         -DQ_OS_UNIX \
         -DQ_PROCESSOR_X86_64 \
         -DHAVE_GEOLOCATION \
         -DHAVE_MEDIA_PLAYER \
         -DHAVE_QWEBENGINE \
         -DHAVE_QTMULTIMEDIA \
         -DHAVE_OPENGL \
         -DHAVE_MYSQLSUPPORT \
         -DHAVE_INTERNALMYSQL \
         -DHAVE_GPHOTO2 \
         -DHAVE_PANORAMA \
         -DHAVE_X11 \
         -DHAVE_X265 \
         -DHAVE_DBUS \
         -DHAVE_LENSFUN \
         -DHAVE_HEIF \
         -DHAVE_EIGEN3 \
         -DHAVE_JASPER \
         -DHAVE_IMAGE_MAGICK \
         -D_XMP_SUPPORT_ \
         --verbose \
         --std=c++17 \
         --library=qt.cfg \
         --library=opencv2.cfg \
         --library=boost.cfg \
         --library=kde.cfg \
         --inline-suppr \
         --xml-version=2 \
         --platform=unix64 \
         --enable=all \
         --check-level=exhaustive \
         --suppress=*:*cimg*.h \
         --suppress=*:*libraw*.h \
         --suppress=*:*libpgf*.h \
         --suppress=*:*upnpsdk*.h \
         --suppress=*:*o2*.h \
         --suppress=*:*lqr*.h \
         --suppress=*:*libjpeg*.h \
         --suppress=*:*dng_sdk*.h \
         --suppress=*:*xmp_sdk/*.h \
         --suppress=*:*qav*.h \
         --suppress=missingIncludeSystem \
         --suppress=missingInclude \
         --suppress=variableScope \
         --suppress=purgedConfiguration \
         --suppress=toomanyconfigs \
         --suppress=unreadVariable \
         --suppress=unusedVariable \
         --suppress=unusedStructMember \
         --suppress=unusedPrivateFunction \
         --suppress=unknownMacro \
         --suppress=unmatchedSuppression:* \
         --suppress=unassignedVariable \
         --suppress=class_X_Y \
         --suppress=ConfigurationNotChecked \
         --suppress=useStlAlgorithm \
         --suppress=cstyleCast \
         --suppress=constVariablePointer \
         --suppress=shadowFunction \
         --output-file=report.cppcheck.xml \
         $IGNORE_DIRS \
         $INCLUDE_DIRS \
         ../../core

cppcheck-htmlreport --file=report.cppcheck.xml \
                    --report-dir=$REPORT_DIR \
                    --source-dir=. \
                    --title=$TITLE

if [[ $1 == "--webupdate" ]] ; then

    # update www.digikam.org report section.
    updateReportToWebsite "cppcheck" $REPORT_DIR $TITLE $(parseGitBranch)

fi

cd $ORIG_DIR

