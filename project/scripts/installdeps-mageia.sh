#!/bin/bash

# Script to install dependencies under Mageia Linux to compile and hack digiKam.
# This script must be run as sudo
#
# Requirements installed are:
#
# - Developement packages to compile source code.
# - API doc compilation.
# - Hanbook compilation.
# - Cross-compiling dependencies for Windows.
# - Run-time dependencies.
# - Debug dependencies.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

# Check root as root

if [[ $EUID -ne 0 ]]; then
    echo "This script should be run as root using sudo command."
    exit 1
else
    echo "Check run as root passed..."
fi

# Enable Tainted RPM media

urpmi.update --no-ignore "Tainted Release"
urpmi.update --no-ignore "Tainted Updates"

# Install all RPMs

urpmi --auto \
      git \
      subversion \
      unrar \
      scons \
      cmake \
      ccache \
      icoutils \
      valgrind \
      cppcheck \
      python3-mako \
      python3-pygments \
      python3-beautifulsoup4 \
      python3-soupsieve \
      perl-Tie-IxHash \
      perl-libxml-perl \
      perl-JSON-XS \
      doxygen \
      hugin \
      kate \
      ktexteditor-plugins \
      extra-cmake-modules \
      gcc-c++ \
      gperf \
      ruby \
      bison \
      flex \
      mingw32-nsis \
      clang-analyzer \
      intltool \
      lzip \
      eigen3-devel \
      lensfun-devel \
      marble-devel \
      marble-data \
      opencv-devel \
      libasan-devel \
      libgomp-devel \
      lib64llvm-devel \
      lib64clang-devel \
      lib64expat-devel \
      lib64lcms2-devel \
      lib64qtav-devel \
      lib64ffmpeg-devel \
      lib64boost-devel \
      lib64gphoto-devel \
      lib64sane1-devel \
      lib64jasper-devel \
      lib64xslt-devel \
      lib64exiv2-devel \
      lib64xml2-devel \
      lib64jpeg-devel \
      lib64png-devel \
      lib64tiff-devel \
      lib64lqr-devel \
      lib64fftw-devel \
      lib64curl-devel \
      lib64magick-devel \
      qtbase5-common-devel \
      lib64qt5core-devel \
      lib64qt5widgets-devel \
      lib64qt5x11extras-devel \
      lib64qt5test-devel \
      lib64qt5xml-devel \
      lib64qt5xmlpatterns-devel \
      lib64qt5concurrent-devel \
      lib64qt5opengl-devel \
      lib64qt5printsupport-devel \
      lib64qt5webengine-devel \
      lib64qt5sql-devel \
      lib64qt5svg-devel \
      lib64kf5sane-devel \
      lib64kf5xmlgui-devel \
      lib64kf5threadweaver-devel \
      lib64kf5kio-devel \
      lib64kf5notifications-devel \
      lib64kf5notifyconfig-devel \
      lib64kf5filemetadata-devel \
      lib64kf5doctools-devel \
      lib64kf5calendarcore-devel
      
