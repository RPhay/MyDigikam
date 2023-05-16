#! /bin/bash

# Script to build a bundle MXE installation with all digiKam low level dependencies in a dedicated directory.
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
exec > >(tee ./logs/build-mxe.full.log) 2>&1

#################################################################################################

echo "01-build-mxe.sh : build a bundle MXE install with digiKam dependencies."
echo "-----------------------------------------------------------------------"

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

export PATH=$MXE_BUILDROOT/usr/bin:$MXE_INSTALL_PREFIX/qt5/bin:$PATH

###############################################################################################
# Check if a previous bundle already exist

CONTINUE_INSTALL=0

if [ -d "$MXE_BUILDROOT" ] ; then

    read -p "$MXE_BUILDROOT already exist. Do you want to remove it or to continue an aborted previous installation ? [(r)emove/(c)ontinue/(s)top] " answer

    if echo "$answer" | grep -iq "^r" ;then

        echo "---------- Removing existing $MXE_BUILDROOT"
#        chmod +w "$MXE_BUILDROOT/usr/readonly"
#        chattr -i "$MXE_BUILDROOT/usr/readonly/.gitkeep"
        rm -rf "$MXE_BUILDROOT"

    elif echo "$answer" | grep -iq "^c" ;then

        echo "---------- Continue aborted previous installation in $MXE_BUILDROOT"
        CONTINUE_INSTALL=1

    else

        echo "---------- Aborting..."
        exit;

    fi

fi

if [[ $CONTINUE_INSTALL == 0 ]]; then

    #################################################################################################
    # Checkout latest MXE from github

    git clone $MXE_GIT_URL $MXE_BUILDROOT

fi

#################################################################################################
# MXE git revision to use

cd $MXE_BUILDROOT

if [[ $MXE_GIT_REVISION == "master" ]]; then
    echo -e "\n"
    echo "---------- Updating MXE"
    git pull
else
    echo -e "\n"
    echo "---------- Checkout MXE revision to $MXE_GIT_REVISION"
    git checkout $MXE_GIT_REVISION
fi

#################################################################################################

if [[ $MXE_GCC_VERSION != "default" ]]; then

    # Use a specific gcc version with MXE
    echo -e "\n"
    echo "---------- Building cross-compiler for MXE"
    make MXE_TARGETS=$MXE_BUILD_TARGETS cc MXE_PLUGIN_DIRS="plugins/gcc$MXE_GCC_VERSION"

    echo -e "\n"
    echo "---------- Building digiKam low level dependencies with GCC version $MXE_GCC_VERSION"

    # Switch to a more recent gcc version
    echo "override MXE_PLUGIN_DIRS += plugins/gcc$MXE_GCC_VERSION" >> settings.mk

fi

#################################################################################################
# Dependencies build and installation

make MXE_TARGETS=$MXE_BUILD_TARGETS \
     openssl \
     cmake \
     yasm \
     gettext \
     freeglut \
     libxml2 \
     libxslt \
     libjpeg-turbo \
     libpng \
     djvulibre \
     tiff \
     boost \
     expat \
     lcms \
     liblqr-1 \
     eigen \
     jasper \
     zlib \
     bzip2 \
     mman-win32 \
     pthreads \
     libgphoto2 \
     icu4c \
     brotli \
     qtbase qtbase_CONFIGURE_OPTS=-icu \
     qtnetworkauth \
     qttranslations \
     qtimageformats \
     qtnetworkauth \
     qttools \
     qtwinextras \
     qtscript \
     qtwebkit \
     fdk-aac \
     x264 \
     x265 \
     xvidcore \
     libvpx \
     theora \
     vorbis \
     opencore-amr \
     librtmp \
     opus \
     speex \
     lame \
     freetype \
     libass \
     openal \
     fftw \
     libltdl \
     openexr \
     nsis

#     libical \

echo -e "\n"

cd $ORIG_WD
cp -f ../../scripts/create_manifest.sh $MXE_BUILDROOT
cd $MXE_BUILDROOT
$MXE_BUILDROOT/create_manifest.sh $MXE_BUILDROOT mxe
cp $MXE_BUILDROOT/mxe_manifest.txt $ORIG_WD/data/

#################################################################################################

echo -e "\n"
echo "---------- Building digiKam 3rd-party dependencies with MXE"

# Create the build dir for the 3rdparty deps
if [ ! -d $BUILDING_DIR ] ; then
    mkdir -p $BUILDING_DIR
fi
if [ ! -d $DOWNLOAD_DIR ] ; then
    mkdir -p $DOWNLOAD_DIR
fi

# For legacy compatibility with bootstrap.mxe
ln -s $MXE_ROOT_DIR $ORIG_WD/build.win64

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

# Low level libraries
# NOTE: The order to compile each component here is very important.

${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_libde265    -- -j$CPU_CORES
${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_libjxl      -- -j$CPU_CORES
${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_libaom      -- -j$CPU_CORES
${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_libavif     -- -j$CPU_CORES
${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_ffmpeg      -- -j$CPU_CORES
${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_imagemagick -- -j$CPU_CORES
${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_opencv      -- -j$CPU_CORES
${MXE_BUILD_TARGETS}-cmake --build . --config RelWithDebInfo --target ext_drmingw     -- -j$CPU_CORES

#################################################################################################

export PATH=$ORIG_PATH

TerminateScript
