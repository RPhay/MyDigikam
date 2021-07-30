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

# Install dependencies to Checkout Source Code

sudo apt-get install -y git                          
echo "-------------------------------------------------------------------"

                                   
sudo apt-get install -y perl                       
echo "-------------------------------------------------------------------"

# Install required dependencies to Compile And Link Source Code

required_packages=("cmake"                   # To Compile   Source Code
                   "extra-cmake-modules"     # To Compile   Source Code

                   "libqt5core5a"            # Qt 5 core module
                   "libqt5gui5"              # Qt 5 Gui module   
                   "libqt5widgets5"          # Qt 5 widget module
                   "libqt5network5"          # Qt 5 network module
                   "libqt5sql5"              # Qt 5 SQL module 
                   "libqt5xml5"              # Qt 5 XML module 
                   "libqt5concurrent5"       # Qt 5 Concurrent module 
                   "libqt5printsupport5"     # Qt 5 PrintSupport module 
                   "libqt5svg5"              # Qt 5 Svg module 
                   "libqt5webengine5"        # Qt 5 webengine module 
                   
                   "libkf5config-dev"        # Configuration settings framework for Qt
                   "libkf5xmlgui-dev"        # User configurable main windows
                   "libkf5i18n-dev"          # Avanced internationalization framework  
                   "libkf5windowsystem-dev"  # Development files for kwindowsystem
                   "libkf5service-dev"       # Development files for kservice   
                   "libkf5solid-dev"         # Qt library to query and control hardware
                   "libkf5coreaddons-dev"    # Development files - KDE Frameworks 5 addons to QtCore

                   "libopencv-dev"           # Development files for opencv   
                   "libpthread-stubs0-dev"   # Development files for pthread
                   "libtiff-dev"             # Tag image file format library  
                   "libpng-dev"              # PNG library   
                   "libjpeg-dev"             # JPEG library
                   "libboost-all-dev"        # Boots C++ Libraries   
                   "liblcms2-dev"            # Little CMS 2 color management library   
                   "libexpat1-dev"           # XML parsing C library
                   "libexiv2-dev"            # Metadata manipulation library
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

<<<<<<< HEAD
=======

# Install optional dependencies to Compile And Link Source Code

optional_packages=("ruby"
                   "subversion"
                   "valgrind"
                   "lzip"
                   "gzip"
                   "unzip"
                   "gperf"
                   "intltool"
                   "unrar"
                   "scons"
                   "icoutils"
                   "gcc-mingw-w64"                   
                   "python3-mako"
                   "python3-pygments"
                   "python3-bs4"
                   "python3-soupsieve"
                   "cppcheck"
                   "clang"
                   "doxygen"
                   "kate"
                   "ktexteditor-katepart"
                   "ccache"
                   "build-essential"
                   "hugin"
                   "bison"                              # >= 2.5.0
                   "flex"                               # >= 2.5.0
                   "wget"                              
                   "coreutils"
                   "dmg2img"
                   "tesseract-ocr"
                   "libeigen3-dev"                      # >= 3.2
                   "liblensfun-dev"                     # >= 0.2.8
                   "libmarble-dev"                      # >= 0.22
                   "marble-data"                        # >= 5.0
                   "libasan4"
                   "libgomp1" 
                   "llvm"
                   "libclang-dev"                      
                   "libqtav-dev"                        # >= 1.12.0
                   "ffmpeg"                             # >= 3.3.x    
                   "libgphoto2-dev"                     # >= 2.5
                   "libsane-dev"                        # >= 5.0.0
                   "libjasper-dev"                      # >= 1.900.1
                   "libxslt-dev"                        # >= 1.1.0
                   "libxml2-dev"                        # >= 2.7.0
                   "libtiff-dev"                        # >= 4.0
                   "liblqr-dev"                         # >= 0.4.2
                   "fftw-dev" 
                   "curl" 
                   "libx265-dev"                        # >= 2.2 
                   "libmagick++-dev"                    # >= 6.7.0    
                   "libqt5x11extras5-dev"               # >= 5.9
                   "libqt5test5"                        # >= 5.9
                   "libqt5xml5"                         # >= 5.9    
                   "libqt5xmlpatterns5-dev"             # >= 5.9
                   "libqt5opengl5-dev"                  # >= 5.9
                   "libkf5sane-dev"                     # >= 5.5.0    
                   "libkf5threadweaver-dev"             # >= 5.5.0
                   "libkf5kio-dev"                      # >= 5.5.0              
                   "libkf5notifications-dev"            # >= 5.5.0
                   "libkf5notifyconfig-dev"             # >= 5.5.0
                   "libkf5filemetadata-dev"             # >= 5.5.0
                   "libkf5calendarcore-dev"             # >= 5.5.0
                   "libkf5akonadicontact-dev"           # >= 4.89.0
                   "libkf5doctools-dev" 
                    #TODO add new optinal packages
                    )    
    

# for pkg in ${optional_packages[@]}; do
#     sudo apt-get install -y ${pkg}
#     echo "-------------------------------------------------------------------"
# done      
>>>>>>> 78496f9251... comment the recommended version for optional deps
