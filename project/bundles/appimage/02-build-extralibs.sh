#!/bin/bash

# Script to build extra libraries using Linux host.
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

#################################################################################################
# Manage script traces to log file

mkdir -p ./logs
exec > >(tee ./logs/build-extralibs.full.log) 2>&1

#################################################################################################

echo "02-build-extralibs.sh : build extra libraries."
echo "----------------------------------------------"

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

#################################################################################################

cd $BUILDING_DIR

rm -rf $BUILDING_DIR/* || true

/opt/cmake/bin/cmake $ORIG_WD/../3rdparty \
      -DCMAKE_INSTALL_PREFIX:PATH=/usr \
      -DINSTALL_ROOT=/usr \
      -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
      -DEXTERNALS_BUILD_DIR=$BUILDING_DIR \
      -DKA_VERSION=$DK_KA_VERSION \
      -DKF5_VERSION=$DK_KF5_VERSION \
      -DENABLE_QTVERSION=$DK_QTVERSION \
      -DENABLE_QTWEBENGINE=$DK_QTWEBENGINE

# NOTE: The order to compile each component here is very important.

# core KF5 frameworks dependencies
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_extra-cmake-modules -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kconfig             -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_breeze-icons        -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kcoreaddons         -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kwindowsystem       -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_solid               -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_threadweaver        -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_karchive            -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kdbusaddons         -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_ki18n               -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kcrash              -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kcodecs             -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kauth               -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kguiaddons          -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kwidgetsaddons      -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kitemviews          -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kcompletion         -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kconfigwidgets      -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kiconthemes         -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kservice            -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kglobalaccel        -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kxmlgui             -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kbookmarks          -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kimageformats       -- -j$CPU_CORES

# Extra support for digiKam

# libksane support
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_libksane            -- -j$CPU_CORES

# Desktop integration support
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kjobwidgets         -- -j$CPU_CORES

/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kio                 -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_knotifyconfig       -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_knotifications      -- -j$CPU_CORES

# TODO: not yet ported to Qt6

if [[ $DK_QTVERSION == 5.* ]] ; then

    # Geolocation support
    /opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_marble              -- -j$CPU_CORES

fi

# Calendar support
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_kcalendarcore       -- -j$CPU_CORES


# Platform Input Context Qt plugin
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_fcitx-qt            -- -j$CPU_CORES

#################################################################################################

TerminateScript
