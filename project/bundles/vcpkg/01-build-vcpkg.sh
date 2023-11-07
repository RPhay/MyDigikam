#! /bin/bash

# Script to build a bundle VCPKG installation with all digiKam low level dependencies in a dedicated directory.
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
exec > >(tee ./logs/build-vcpkg.full.log) 2>&1

#################################################################################################

echo "01-build-vcpkg.sh : build a bundle VCPKG install with digiKam dependencies."
echo "---------------------------------------------------------------------------"

#################################################################################################
# Pre-processing checks

. ./config.sh
. ./common.sh
StartScript
ChecksCPUCores
RegisterRemoteServers

#################################################################################################

# Paths rules
ORIG_PATH="$PATH"
ORIG_WD="`pwd`"

###############################################################################################
# Check if a previous bundle already exist

CONTINUE_INSTALL=0

if [ -d "$VCPKG_DIR" ] ; then

    read -p "$VCPKG_DIR already exist. Do you want to remove it or to continue an aborted previous installation ? [(r)emove/(c)ontinue/(s)top] " answer

    if echo "$answer" | grep -iq "^r" ;then

        echo "---------- Removing existing $MXE_BUILDROOT"
        rm -rf "$VCPKG_DIR"

    elif echo "$answer" | grep -iq "^c" ;then

        echo "---------- Continue aborted previous installation in $VCPKG_DIR"
        CONTINUE_INSTALL=1

    else

        echo "---------- Aborting..."
        exit;

    fi

fi

if [[ $CONTINUE_INSTALL == 0 ]]; then

    #################################################################################################
    # Checkout latest VCPKG from github

    git clone https://github.com/Microsoft/vcpkg.git $VCPKG_DIR

    $VCPKG_DIR/bootstrap-vcpkg.bat

fi

#################################################################################################
# MXE git revision to use

cd $VCPKG_DIR

git pull
$VCPKG_DIR/vcpkg update

#################################################################################################
# Dependencies build and installation

$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install openssl
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install pthreads
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install gettext
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install icu
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libpng
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install tiff
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libmysql
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libjpeg-turbo
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install jasper
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libde265
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libjxl
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install aom
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libavif
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libheif
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install freeglut
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install brotli
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install zlib
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install bzip2
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install ffmpeg
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install qt

$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install opencv
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install boost
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install lcms
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install eigen3
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install expat
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libxml2
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libxslt
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install exiv2
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libical
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install fftw3
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install openexr
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install gperf
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install libsnoretoast
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install hunspell
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install lensfun
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install graphicsmagick
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install openal-soft
$VCPKG_DIR/vcpkg ${VCPKG_COMMON_OPTIONS[@]} install curl

echo -e "\n"

cd $ORIG_WD
cp -f ../../scripts/create_manifest.sh $VCPKG_DIR
cd $VCPKG_DIR
$VCPKG_DIR/create_manifest.sh $VCPKG_DIR vcpkg
cp $VCPKG_DIR/vcpkg_manifest.txt $ORIG_WD/data/

if [ ] ; then

#################################################################################################

echo -e "\n"
echo "---------- Building digiKam 3rd-party dependencies with VCPKG"

# Create the build dir for the 3rdparty deps

if [ ! -d $BUILDING_DIR/dk_cmake ] ; then
    mkdir -p $BUILDING_DIR/dk_cmake
fi

if [ ! -d $DOWNLOAD_DIR ] ; then
    mkdir -p $DOWNLOAD_DIR
fi

cd $BUILDING_DIR/dk_cmake
rm -rf $BUILDING_DIR/dk_cmake/* || true

cmake $ORIG_WD/../3rdparty \
                           -DCMAKE_TOOLCHAIN_FILE=$VCKPG_DIR/scripts/buildsystems/vcpkg.cmake \
                           -DVCPKG_TARGET_TRIPLET=$VCPKG_TRIPLET \
                           -DCMAKE_BUILD_TYPE=RelWithDebInfo \
                           -DCMAKE_COLOR_MAKEFILE=ON \
                           -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/$VCPKG_TRIPLET \
                           -DINSTALL_ROOT=$INSTALL_DIR/$VCPKG_TRIPLET \
                           -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
                           -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
                           -DKA_VERSION=$DK_KA_VERSION \
                           -DKP_VERSION=$DK_KP_VERSION \
                           -DKDE_VERSION=$DK_KDE_VERSION

#                           -DCMAKE_FIND_PREFIX_PATH=${CMAKE_PREFIX_PATH} \
#                           -DCMAKE_SYSTEM_INCLUDE_PATH=${CMAKE_PREFIX_PATH}/include \
#                           -DCMAKE_INCLUDE_PATH=${CMAKE_PREFIX_PATH}/include \
#                           -DCMAKE_LIBRARY_PATH=${CMAKE_PREFIX_PATH}/lib \

# Low level libraries
# NOTE: The order to compile each component here is very important.

cmake --build . --parallel --config RelWithDebInfo --target ext_imagemagick

#################################################################################################
fi

export PATH=$ORIG_PATH

TerminateScript
