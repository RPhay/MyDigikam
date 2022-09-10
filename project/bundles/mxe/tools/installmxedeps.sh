#! /bin/bash

# Script to install extra MXE dependency pass as name as first argument (ex: "heif")
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

    echo "Pass the name of extra MXE dependency to install as first argument (aka 'heif' for ex.)"
    exit
fi

#################################################################################################
# Pre-processing checks

cd ..

. ./config.sh
. ./common.sh
StartScript
ChecksCPUCores
RegisterRemoteServers

#################################################################################################

# Paths rules
ORIG_PATH="$PATH"
ORIG_WD="`pwd`"

export PATH=$MXE_BUILDROOT/usr/bin:$MXE_INSTALL_PREFIX/qt5/bin:$PATH

#################################################################################################

# Create the build dir for the 3rdparty deps
if [ ! -d $BUILDING_DIR ] ; then
    mkdir -p $BUILDING_DIR
fi
if [ ! -d $DOWNLOAD_DIR ] ; then
    mkdir -p $DOWNLOAD_DIR
fi

cd $BUILDING_DIR
rm -rf $BUILDING_DIR/* || true

${MXE_BUILD_TARGETS}-cmake $ORIG_WD/../3rdparty \
                           -DMXE_TOOLCHAIN=${MXE_TOOLCHAIN} \
                           -DMXE_BUILDROOT=${MXE_BUILDROOT} \
                           -DMXE_BUILD_TARGETS=${MXE_BUILD_TARGETS} \
                           -DMXE_ARCHBITS=${MXE_ARCHBITS} \
                           -DMXE_INSTALL_PREFIX=${MXE_INSTALL_PREFIX} \
                           -DCMAKE_BUILD_TYPE=RelWithDebInfo \
                           -DCMAKE_COLOR_MAKEFILE=ON \
                           -DCMAKE_INSTALL_PREFIX=${MXE_INSTALL_PREFIX} \
                           -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
                           -DCMAKE_TOOLCHAIN_FILE=${MXE_TOOLCHAIN} \
                           -DCMAKE_FIND_PREFIX_PATH=${CMAKE_PREFIX_PATH} \
                           -DCMAKE_SYSTEM_INCLUDE_PATH=${CMAKE_PREFIX_PATH}/include \
                           -DCMAKE_INCLUDE_PATH=${CMAKE_PREFIX_PATH}/include \
                           -DCMAKE_LIBRARY_PATH=${CMAKE_PREFIX_PATH}/lib \
                           -DZLIB_ROOT=${CMAKE_PREFIX_PATH} \
                           -DINSTALL_ROOT=${MXE_INSTALL_PREFIX} \
                           -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
                           -DKA_VERSION=$DK_KA_VERSION \
                           -DKF5_VERSION=$DK_KF5_VERSION \
                           -DENABLE_QTVERSION=$DK_QTVERSION \
                           -DENABLE_QTWEBENGINE=$DK_QTWEBENGINE

${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_$1    -- -j$CPU_CORES

#################################################################################################

export PATH=$ORIG_PATH

TerminateScript
