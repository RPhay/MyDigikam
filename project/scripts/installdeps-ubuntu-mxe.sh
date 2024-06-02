#!/bin/bash

# Script to install dependencies under Ubuntu Linux to compile digiKam.
# This script must be run as sudo
# *Ubuntu* compatible version >= 18.04
#
# SPDX-FileCopyrightText: 2021      by TRAN Quoc Hung <quochungtran1999 at gmail dot com>
# SPDX-FileCopyrightText: 2021      by Surya K M      <suryakm_is20 dot rvitm@rvei dot edu dot in>
# SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Check OS name and version.

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

echo "Check the list of dependencies in the online doc API : https://www.digikam.org/api/index.html#externaldeps"
echo "-------------------------------------------------------------------"

# for downloading package information from all configured sources.'

sudo apt-get update
sudo apt-get upgrade
sudo apt install software-properties-common

# benefit from a higher version of certain software , update the key

sudo apt-key adv --refresh-keys --keyserver keyserver.ubuntu.com
sudo add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main"

# To fix GPP key error with some reporsitories
# See: https://www.skyminds.net/linux-resoudre-les-erreurs-communes-de-cle-gpg-dans-apt/

sudo apt-get update 2>&1 | \
    sed -ne 's?^.*NO_PUBKEY ??p' | \
    xargs -r -- sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys

# Install dependencies to Checkout Source Code

sudo apt-get install -y git

echo "-------------------------------------------------------------------"

sudo apt-get install -y perl

echo "-------------------------------------------------------------------"

# Install required dependencies to Compile And Link Source Code

required_packages=("cmake"                   # To Compile Source Code
                   "ninja-build"             # To Compile Source Code
                   "build-essential"         # To Compile Source Code
                   "extra-cmake-modules"
                   "qtbase5-dev"
                  )

for pkg in ${required_packages[@]}; do

    sudo apt-get install -y ${pkg}

    current_version=$(dpkg-query --showformat='${Version}' --show ${pkg})

    case "${pkg}" in
    "cmake")
        required_version=3.3.2
        ;;
    "extra-cmake-modules")
        required_version=5.5.0
        ;;
    "build-essential")
        required_version=7.2.0
        ;;
    "libqt5core5a")
        required_version=5.9.0
        ;;
    "libkf5config-dev")
        required_version=5.5.0
        ;;
    "libopencv-dev")
        required_version=3.3.0
        ;;
    "libpthread-stubs0-dev")
        required_version=2.0.0
        ;;
    "libtiff-dev")
        required_version=4.0.0
        ;;
    "libpng-dev")
        required_version=1.6.0
        ;;
    "libjpeg-dev")
        required_version=6b
        ;;
    "libboost-all-dev")
        required_version=1.55.0
        ;;
    "liblcms2-dev")
        required_version=2.0.0
        ;;
    "libexpat1-dev")
        required_version=2.1.0
        ;;
    "libexiv2-dev")
        required_version=0.27.0
        ;;
    esac

    echo $current_version

    if $(dpkg --compare-versions "$current_version" "lt" "$required_version"); then
            echo "less than $required_version";
            echo "please upgrade newer version or another packages";
    else
            echo "greater than $required_version ............. accepted";
    fi

    echo "-------------------------------------------------------------------"

done

# install Python 3.7
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt install -y python3.7
ln -s /usr/bin/python3 /usr/bin/python3.7

# Install optional dependencies to Compile And Link Source Code

optional_packages=("ruby"                               # For i18n extraction
                   "subversion"                         # For i18n extraction
                   "keychain"                           # For git-ssh
                   "ssh-askpass"                        # For git-ssh
                   "lzip"                               # For CI/CD
                   "gzip"                               # For CI/CD
                   "unzip"                              # For CI/CD
                   "unrar"                              # For CI/CD
                   "autoconf"                           # For MXE build
                   "automake"                           # For MXE build
                   "autopoint"                          # For MXE build
                   "bash"                               # For MXE build
                   "bison"                              # For MXE build
                   "bzip2"                              # For MXE build
                   "flex"                               # For MXE build
                   "gcc-mingw-w64"                      # For MXE build
                   "g++"                                # For MXE build
                   "g++-multilib"                       # For MXE build
                   "gettext"                            # For MXE build
                   "git"                                # For MXE build
                   "gperf"                              # For MXE build
                   "icoutils"                           # For MXE build
                   "intltool"                           # For MXE build
                   "libc6-dev-i386"                     # For MXE build
                   "libgdk-pixbuf2.0-dev"               # For MXE build
                   "gtk-doc-tools"                      # For MXE build
                   "libltdl-dev"                        # For MXE build
                   "libgl-dev"                          # For MXE build
                   "libssl-dev"                         # For MXE build
                   "libtool"                            # For MXE build
                   "libtool-bin"                        # For MXE build
                   "libxml-parser-perl"                 # For MXE build
                   "lzip"                               # For MXE build
                   "make"                               # For MXE build
                   "openssl"                            # For MXE build
                   "p7zip-full"                         # For MXE build
                   "patch"                              # For MXE build
                   "perl"                               # For MXE build
                   "python3"                            # For MXE build
                   "python3-mako"                       # For MXE build
                   "python3-pkg-resources"              # For MXE build
                   "python-mako"                        # For MXE build
                   "python-pkg-resources"               # For MXE build
                   "python-is-python3"                  # For MXE build
                   "python3-setuptools"                 # For Lensfun
                   "ruby"                               # For MXE build
                   "scons"                              # For MXE build
                   "sed"                                # For MXE build
                   "unzip"                              # For MXE build
                   "wget"                               # For MXE build
                   "xz-utils"                           # For MXE build
                   "ccache"                             # For compiling
                   "bison"                              # For Qt build (>= 2.5.0)
                   "flex"                               # For compiling (>= 2.5.0)
                   "curl"                               # For CI/CD
                   "wget"                               # For CI/CD
                   "coreutils"                          # For CI/CD
                    #TODO add new optional packages
                    )


for pkg in ${optional_packages[@]}; do
    sudo apt-get install -y ${pkg}
    echo "-------------------------------------------------------------------"
done

