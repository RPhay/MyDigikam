#!/bin/bash

# Copyright (c) 2013-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

########################################################################

# Absolute path where are downloaded all tarballs to compile.
DOWNLOAD_DIR="`pwd`/temp.dwnld"

# Absolute path where are compiled all tarballs
BUILDING_DIR="`pwd`/temp.build"

########################################################################

# Target macOS achitecture: "x86_64" for Intel 64 bits, or "arm64" for Apple Silicon 64 bits.
ARCH_TARGET="x86_64"

if [[ $ARCH_TARGET = "x86_64" ]] ; then

    # Minimum MacOS target for backward binary compatibility with Intel CPU
    # This require to install older MacOS SDKs with Xcode.
    # See this url to download a older SDK archive :
    #
    # https://github.com/alexey-lysiuk/macos-sdk
    #
    # Uncompress the archive to /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/
    # and adjust the property "MinimumSDKVersion" from /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Info.plist
    # sudo /usr/libexec/PlistBuddy -c "Set MinimumSDKVersion 10.13" /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Info.plist
    #
    # Possible values:
    # 12.0  : Monterey     :          : Qt                                            5.13, 5.14, 5.15
    # 11.0  : BigSur       :          : Qt                                      5.12, 5.13, 5.14, 5.15
    # 10.15 : Catalina     :          : Qt                                5.11, 5.12, 5.13, 5.14
    # 10.14 : Mojave       : tested   : Qt                     5.9, 5.10, 5.11, 5.12, 5.13, 5.14
    # 10.13 : High Sierra  : tested   : Qt                     5.9, 5.10, 5.11, 5.12, 5.13, 5.14
    # 10.12 : Sierra       : tested   : Qt                5.8, 5.9, 5.10, 5.11, 5.12, 5.13
    # 10.11 : El Capitan   : tested   : Qt 5.5, 5.6, 5.7, 5.8, 5.9, 5.10, 5.11
    # 10.10 : Yosemite     : tested   : Qt 5.5, 5.6, 5.7, 5.8, 5.9
    # 10.9  : Mavericks    : tested   : Qt 5.5, 5.6, 5.7, 5.8
    # 10.8  : MountainLion : tested   : Qt 5.5, 5.6, 5.7
    # 10.7  : Lion         : untested : Qt 5.5, 5.6
    # 10.6  : SnowLeopard  : untested : ???
    # Older values cannot be set as it do not support x86_64.
    OSX_MIN_TARGET="10.13"

elif [[ $ARCH_TARGET = "arm64" ]] ; then

    # Apple Silicon is supported since macOS BigSur
    OSX_MIN_TARGET="11.0"

else

    echo "Unsuported or invalid target architecture..."
    exit -1

fi

# Directory to build and install Macports packages.
INSTALL_PREFIX="/opt/digikam.org.$ARCH_TARGET"
# Local install prefix which do not require sudo right
#INSTALL_PREFIX="`pwd`/digikam.org.$ARCH_TARGET"

# Directory where target bundle contents will be installed.
RELOCATE_PREFIX="/Applications/digiKam.org"

# Macports configuration
MP_URL="https://distfiles.macports.org/MacPorts/"
MP_BUILDTEMP=~/mptemp

# Uncomment this line to force a specific version of Macports to use, else lastest will be used.
#MP_VERSION="2.3.3"

########################################################################

# URL to git repository to checkout digiKam source code
DK_GITURL="git@invent.kde.org:graphics/digikam.git"

# digiKam tarball information
DK_URL="http://download.kde.org/stable/digikam"

# Location to build source code.
DK_BUILDTEMP=~/dktemp

# digiKam tag version from git. Official tarball do not include extra shared libraries.
# The list of tags can be listed with this url: https://invent.kde.org/graphics/digikam/-/tags
# If you want to package current implementation from git, use "master" as tag.
#DK_VERSION=v7.2.0-beta1
DK_VERSION=master
#DK_VERSION=development/dplugins

# Installer sub version to differentiates newer updates of the installer itself, even if the underlying application hasn’t changed.
#DK_SUBVER="-01"

# Installer will include or not digiKam debug symbols
DK_DEBUG=0

# Option to use QtWebEngine instead QtWebkit
DK_QTWEBENGINE=1

# Sign bundles with GPG. Passphrase must be hosted in ~/.gnupg/dkorg-gpg-pwd.txt
DK_SIGN=0

# Upload automatically bundle to files.kde.org (pre-release only).
DK_UPLOAD=1
DK_UPLOADURL="digikam@deino.kde.org"
DK_UPLOADDIR="/srv/archives/files/digikam/"
