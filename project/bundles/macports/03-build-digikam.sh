#! /bin/bash

# Script to build digiKam using MacPorts
# This script must be run as sudo
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Ask to run as root
(( EUID != 0 )) && exec sudo -- "$0" "$@"

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

#################################################################################################
# Manage script traces to log file

mkdir -p ./logs
exec > >(tee ./logs/build-digikam.full.log) 2>&1

#################################################################################################

echo "03-build-digikam.sh : build digiKam using MacPorts."
echo "---------------------------------------------------"

#################################################################################################
# Pre-processing checks

. ./config.sh
. ./common.sh
StartScript
ChecksRunAsRoot
ChecksXCodeCLI
ChecksCPUCores
OsxCodeName
#RegisterRemoteServers

#################################################################################################

# Pathes rules
ORIG_PATH="$PATH"
ORIG_WD="`pwd`"

export PATH=$INSTALL_PREFIX/bin:/$INSTALL_PREFIX/sbin:/$INSTALL_PREFIX/libexec/qt5/bin:$ORIG_PATH

#################################################################################################
# Install out-dated dependencies

cd $BUILDING_DIR

rm -rf $BUILDING_DIR/* || true

cmake $ORIG_WD/../3rdparty \
       -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX \
       -DINSTALL_ROOT=$INSTALL_PREFIX \
       -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
       -DENABLE_QTWEBENGINE=$DK_QTWEBENGINE \
       -DMACOSX_DEPLOYMENT_TARGET=$OSX_MIN_TARGET \
       -Wno-dev

cmake --build . --config RelWithDebInfo --target ext_exiv2   -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_qtav    -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_lensfun -- -j$CPU_CORES

#################################################################################################
# Build digiKam in temporary directory and installation

if [ -d "$DK_BUILDTEMP/digikam-$DK_VERSION" ] ; then

    echo "---------- Updating existing $DK_BUILDTEMP"

    cd "$DK_BUILDTEMP"
    cd digikam-$DK_VERSION

    git reset --hard
    git pull

    rm -fr po
    rm -fr build
    mkdir -p build

else

    echo "---------- Creating $DK_BUILDTEMP"
    mkdir -p "$DK_BUILDTEMP"

    if [ $? -ne 0 ] ; then
        echo "---------- Cannot create $DK_BUILDTEMP directory."
        echo "---------- Aborting..."
        exit;
    fi

    cd "$DK_BUILDTEMP"
    echo -e "\n\n"
    echo "---------- Downloading digiKam $DK_VERSION"

    git clone --progress --verbose --branch $DK_VERSION --single-branch $DK_GITURL digikam-$DK_VERSION
    cd digikam-$DK_VERSION

    if [ $? -ne 0 ] ; then
        echo "---------- Cannot clone repositories."
        echo "---------- Aborting..."
        exit;
    fi

    mkdir build

fi

echo -e "\n\n"
echo "---------- Configure digiKam $DK_VERSION"

sed -e "s/DIGIKAMSC_CHECKOUT_PO=OFF/DIGIKAMSC_CHECKOUT_PO=ON/g" ./bootstrap.macports > ./tmp.macports ; mv -f ./tmp.macports ./bootstrap.macports
sed -e "s/DIGIKAMSC_COMPILE_PO=OFF/DIGIKAMSC_COMPILE_PO=ON/g"   ./bootstrap.macports > ./tmp.macports ; mv -f ./tmp.macports ./bootstrap.macports
sed -e "s/DBUILD_TESTING=ON/DBUILD_TESTING=OFF/g"               ./bootstrap.macports > ./tmp.macports ; mv -f ./tmp.macports ./bootstrap.macports
sed -e "s/DENABLE_DBUS=ON/DENABLE_DBUS=OFF/g"                   ./bootstrap.macports > ./tmp.macports ; mv -f ./tmp.macports ./bootstrap.macports

if [[ $DK_QTWEBENGINE = 0 ]] ; then

    sed -e "s/DENABLE_QWEBENGINE=ON/DENABLE_QWEBENGINE=OFF/g"   ./bootstrap.macports > ./tmp.macports ; mv -f ./tmp.macports ./bootstrap.macports

fi

chmod +x ./bootstrap.macports

cp -f $ORIG_WD/fixbundledatapath.sh $DK_BUILDTEMP/digikam-$DK_VERSION

./fixbundledatapath.sh

./bootstrap.macports "$INSTALL_PREFIX" "Debug" "x86_64" "-Wno-dev"

if [ $? -ne 0 ]; then
    echo "---------- Cannot configure digiKam $DK_VERSION."
    echo "---------- Aborting..."
    exit;
fi

echo -e "\n\n"
echo "---------- Building digiKam $DK_VERSION"

cd build
make -j$CPU_CORES

if [ $? -ne 0 ]; then
    echo "---------- Cannot compile digiKam $DK_VERSION."
    echo "---------- Aborting..."
    exit;
fi

cat $DK_BUILDTEMP/digikam-$DK_VERSION/build/core/app/utils/digikam_version.h   | grep "digikam_version\[\]" | awk '{print $6}' | tr -d '";'  > $ORIG_WD/data/RELEASEID.txt
cat $DK_BUILDTEMP/digikam-$DK_VERSION/build/core/app/utils/digikam_builddate.h | grep "define BUILD_DATE"   | awk '{print $3}' | tr -d '"\n' > $ORIG_WD/data/BUILDDATE.txt

echo -e "\n\n"
echo "---------- Installing digiKam $DK_VERSION"
echo -e "\n\n"

make install/fast && cd "$ORIG_WD"

if [ $? -ne 0 ]; then
    echo "---------- Cannot install digiKam $DK_VERSION."
    echo "---------- Aborting..."
    exit;
fi

#################################################################################################
# Install Extra Plugins

cd $BUILDING_DIR

rm -rf $BUILDING_DIR/* || true

cmake $ORIG_WD/../3rdparty \
       -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX \
       -DINSTALL_ROOT=$INSTALL_PREFIX \
       -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
       -Wno-dev

cmake --build . --config RelWithDebInfo --target ext_gmic_qt    -- -j$CPU_CORES
cmake --build . --config RelWithDebInfo --target ext_mosaicwall -- -j$CPU_CORES

mv -f $INSTALL_PREFIX/libexec/qt5/plugins/digikam/editor/*.so $INSTALL_PREFIX/lib/plugins/digikam/editor/
# Install GmicQt plugin debug symbols.
cp -pr $BUILDING_DIR/ext_gmicqt/ext_gmic_qt-prefix/src/ext_gmic_qt-build/Editor_GmicQt_Plugin.so.dSYM $INSTALL_PREFIX/lib/plugins/digikam/editor/ || true

#################################################################################################

export PATH=$ORIG_PATH

TerminateScript
