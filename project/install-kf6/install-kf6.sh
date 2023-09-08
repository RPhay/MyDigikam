#!/bin/bash

# Script to build extra libraries using Linux host.
# This script must be run as sudo
#
# SPDX-FileCopyrightText: 2015-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

#################################################################################################
# Manage script traces to log file

mkdir -p ./logs
exec > >(tee ./logs/build-extralibs.full.log) 2>&1

#################################################################################################
# Pre-processing checks

. ./common.sh
. ./config.sh
ChecksRunAsRoot
StartScript
ChecksCPUCores
HostAdjustments
RegisterRemoteServers

ORIG_WD="`pwd`"

# Create the build dir for the 3rdparty deps
if [ ! -d $BUILDING_DIR ] ; then
    mkdir $BUILDING_DIR
fi

if [ ! -d $DOWNLOAD_DIR ] ; then
    mkdir $DOWNLOAD_DIR
fi

#################################################################################################

cd $BUILDING_DIR

rm -rf $BUILDING_DIR/* || true

cmake $ORIG_WD \
      -DCMAKE_INSTALL_PREFIX:PATH=/usr \
      -DINSTALL_ROOT=/usr \
      -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
      -DEXTERNALS_BUILD_DIR=$BUILDING_DIR \
      -DKA_VERSION=$DK_KA_VERSION \
      -DKP_VERSION=$DK_KP_VERSION \
      -DKDE_VERSION=$DK_KDE_VERSION \
      -DENABLE_QTVERSION=$DK_QTVERSION \
      -Wno-dev

# NOTE: The order to compile each component here is very important.

# core KF5 frameworks dependencies
cmake --build . --config RelWithDebInfo --target ext_extra-cmake-modules -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kconfig             -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_breeze-icons        -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kcoreaddons         -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kwindowsystem       -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_solid               -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_threadweaver        -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_karchive            -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kdbusaddons         -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_ki18n               -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kcrash              -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kcodecs             -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kauth               -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kguiaddons          -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kwidgetsaddons      -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kitemviews          -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kcompletion         -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kcolorscheme        -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kconfigwidgets      -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kiconthemes         -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kservice            -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kglobalaccel        -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kxmlgui             -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kbookmarks          -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kimageformats       -- -j$CPU_CORES

# Extra support for digiKam

# libksane support
cmake --build . --config RelWithDebInfo --target ext_ksanecore           -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_libksane            -- -j$CPU_CORES

# Desktop integration support
cmake --build . --config RelWithDebInfo --target ext_knotifications      -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_kjobwidgets         -- -j$CPU_CORES

cmake --build . --config RelWithDebInfo --target ext_kio                 -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_knotifyconfig       -- -j$CPU_CORES

# Geolocation support
#cmake --build . --config RelWithDebInfo --target ext_marble              -- -j$CPU_CORES

# Calendar support
cmake --build . --config RelWithDebInfo --target ext_kcalendarcore       -- -j$CPU_CORES

#################################################################################################

$ORIG_WD/kf6-create-manifest.sh $ORIG_WD/b

TerminateScript
