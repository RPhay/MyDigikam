#!/bin/bash

# Script to install dependencies under Ubuntu Linux to compile digiKam.
# This script must be run as sudo
#
# Copyright (c) 2021 by Surya K M <suryakm_is20 dot rvitm@rvei dot edu dot in>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

#Check OS name and version.

OS_NAME=$(awk '/DISTRIB_ID=/' /etc/*-release | sed 's/DISTRIB_ID=//' | sed 's/\"//' | sed 's/\"//' | tr '[:upper:]' '[:lower:]')
OS_ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')
OS_VERSION=$(awk '/DISTRIB_RELEASE=/' /etc/*-release | sed 's/DISTRIB_RELEASE=//' | sed 's/[.]0/./')

echo $OS_NAME
echo $OS_ARCH
echo $OS_VERSION


if [[ "$OS_NAME" != "ubuntu" ]] ; then
    echo "Not running Linux ubuntu..."
    exit -1
fi


# for downloading package information from all configured sources.'

sudo apt-get update
sudo apt-get upgrade 

# benefit from a higher version of certain software , update the key

sudo apt-key adv --refresh-keys --keyserver keyserver.ubuntu.com
sudo add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main"


# Install all parkages dependencies

sudo apt-get install -y \
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
      gcc-mingw-w64                 `#  For MXE build.` \
      python3-mako                  `#  For MXE build.` \
      python3-pygments              `#  For Continuous integration.` \
      python3-bs4                   `#  For Continuous integration.` \
      python3-soupsieve             `#  For Continuous integration.` \
      libtie-ixhash-perl            `#  For Continuous integration.` \
      libxml-libxml-perl            `#  For Continuous integration.` \
      libjson-xs-perl               `#  For Continuous integration.` \
      cppcheck                      `#  For Continuous integration.` \
      clang                         `#  For Continuous integration.` \
      doxygen                       `#  To build API DOC.` \
      kate                          `#  Advanced text editor for developers.` \
      ktexteditor-katepart          `#  Advanced text editor for developers.` \
      cmake                         `#  To compile source code.` \
      ccache                        `#  To compile source code.` \
      extra-cmake-modules           `#  To compile source code.` \
      build-essential               `#  To compile source code.` \
      hugin                         `#  Panorama tool.` \
      bison                         `#  Panorama tool.` \
      flex                          `#  Panorama tool.` \
      wget \
      coreutils \
      dmg2img \
      tesseract-ocr \
      libeigen3-dev \
      liblensfun-dev \
      libmarble-dev \
      marble-data \
      libopencv-dev \
      libasan4 \
      libgomp1 \
      llvm \
      libclang-dev \
      libexpat-dev \
      liblcms2-dev \
      libqtav-dev \
      ffmpeg \
      libboost-dev \
      libgphoto2-dev \
      libsane-dev \
      libjasper-dev \
      libxslt-dev \
      libexiv2-dev \
      libxml2-dev \
      libjpeg-dev \
      libpng-dev \
      libtiff-dev \
      liblqr-dev \
      fftw-dev \
      curl \
      libx265-dev \
      libmagick++-dev \
      qt5-default \
      qtbase5-dev \
      libqt5widgets5 \
      libqt5x11extras5-dev \
      libqt5test5 \
      libqt5xml5 \
      libqt5xmlpatterns5-dev \
      libqt5concurrent5 \
      libqt5opengl5-dev \
      libqt5printsupport5 \
      libqt5webengine5 \
      libqt5sql5 \
      libqt5svg5 \
      libkf5sane-dev \
      libkf5xmlgui-dev \
      libkf5threadweaver-dev \
      libkf5kio-dev \
      libkf5notifications-dev \
      libkf5notifyconfig-dev \
      libkf5filemetadata-dev \
      libkf5calendarcore-dev \
      libkf5akonadicontact-dev \
      libkf5doctools-dev
