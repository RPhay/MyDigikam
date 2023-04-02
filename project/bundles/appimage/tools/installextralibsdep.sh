#!/bin/bash

# Script to install extra libs dependency pass as name as first argument (ex: "kimageformats")
# This script must be run as sudo
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

if [ -z "$1" ] ; then

    echo "Pass the name of extra libs dependency to install as first argument (aka 'kimageformats' for ex.)"
    exit
fi

#################################################################################################
# Pre-processing checks

. ../common.sh
. ../config.sh
ChecksRunAsRoot
StartScript
ChecksCPUCores
HostAdjustments
RegisterRemoteServers

ORIG_WD="`pwd`/.."

#################################################################################################

cd $BUILDING_DIR

rm -rf $BUILDING_DIR/* || true

/opt/cmake/bin/cmake $ORIG_WD/../3rdparty \
      -DCMAKE_INSTALL_PREFIX:PATH=/usr \
      -DINSTALL_ROOT=/usr \
      -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
      -DKA_VERSION=$DK_KA_VERSION \
      -DKF5_VERSION=$DK_KF5_VERSION \
      -DENABLE_QTVERSION=$DK_QTVERSION \
      -DENABLE_QTWEBENGINE=$DK_QTWEBENGINE

/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_$1 -- -j$CPU_CORES

#################################################################################################

TerminateScript
