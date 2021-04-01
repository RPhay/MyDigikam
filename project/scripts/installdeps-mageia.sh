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

# Check OS name and version.

OS_NAME=$(awk '/DISTRIB_ID=/' /etc/*-release | sed 's/DISTRIB_ID=//' | sed 's/\"//' | sed 's/\"//' | tr '[:upper:]' '[:lower:]')
OS_ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')
OS_VERSION=$(awk '/DISTRIB_RELEASE=/' /etc/*-release | sed 's/DISTRIB_RELEASE=//' | sed 's/[.]0/./')

echo $OS_NAME
echo $OS_ARCH
echo $OS_VERSION

if [[ "$OS_NAME" != "mageia" ]] ; then
    echo "Not running Linux Mageia..."
    exit -1
fi

# Enable Tainted RPM media

urpmi.update --no-ignore "Tainted Release"
urpmi.update --no-ignore "Tainted Updates"

# Install all RPMs

urpmi --auto \
      git                           `#  source code is hosted on git server.` \
      subversion                    `#  To checkout translation files.` \
      ruby                          `#  To checkout translation files.` \
      valgrind                      `#  To hack at run-time.` \
      lzip                          `#  For MXE build.` \
      gzip                          `#  For MXE build.` \
      unzip                         `#  For MXE build.` \
      gperf                         `#  For MXE build.` \
      intltool                      `#  For MXE build.` \
      unrar                         `#  For MXE build.` \
      scons                         `#  For MXE build.` \
      icoutils                      `#  For MXE build.` \
      mingw32-nsis                  `#  For MXE build.` \
      python3-mako                  `#  For MXE build.` \
      python3-pygments              `#  For Continuous integration.` \
      python3-beautifulsoup4        `#  For Continuous integration.` \
      python3-soupsieve             `#  For Continuous integration.` \
      perl-Tie-IxHash               `#  For Continuous integration.` \
      perl-libxml-perl              `#  For Continuous integration.` \
      perl-JSON-XS                  `#  For Continuous integration.` \
      cppcheck                      `#  For Continuous integration.` \
      clang-analyzer                `#  For Continuous integration.` \
      doxygen                       `#  To build API DOC.` \
      kate                          `#  Advanced text editor for developers.` \
      ktexteditor-plugins           `#  Advanced text editor for developers.` \
      cmake                         `#  To compile source code.` \
      ccache                        `#  To compile source code.` \
      extra-cmake-modules           `#  To compile source code.` \
      gcc-c++                       `#  To compile source code.` \
      hugin                         `#  Panorama tool.` \
      bison                         `#  Panorama tool.` \
      flex                          `#  Panorama tool.` \
      wget \
      coreutils \
      dmg2img \
      tesseract \
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
      lib64kf5calendarcore-devel

if   [[ $OS_VERSION == 8 ]] ; then
    urpmi --auto lib64kf5doctools-devel
elif [[ $OS_VERSION == 7 ]] ; then
    urpmi --auto lib64kdoctools-devel
fi
