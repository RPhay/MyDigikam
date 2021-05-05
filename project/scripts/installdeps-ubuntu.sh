#!/bin/bash

# Script to install dependencies under Mageia Linux to compile digiKam.
# This script must be run as sudo
#
# Copyright (c) 2021 by Surya K M <suryakm_is20 dot rvitm@rvei dot edu dot in>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

#Installing cmake
sudo snap install cmake

#Installing ECM
#Quick Install Steps:
#Step 1
sudo apt-get update -y
#Step 2
sudo apt-get install -y ecm


#Installing  qt5 :-
sudo apt-get install build-essential

sudo apt-get install qtcreator

sudo apt-get install qt5-default

#installing the Qt documentation :-
sudo apt-get install qt5-doc
sudo apt-get install qt5-doc-html qtbase5-doc-html
sudo apt-get install qtbase5-examples

#installing kf5 config in ubuntu :-
sudo apt-get install libkf5config-dev

#installing kf5 xmlgui in ubuntu :-
sudo apt-get install libkf5xmlgui-dev

#installing kf5 i18n in ubuntu :-
sudo apt-get install libkf5i18n-dev

#installing kf5 windowsystem in ubuntu :-
sudo apt-get install libkf5windowsystem-dev

#installing KF5 solid in ubuntu :-
sudo apt-get install libkf5solid-dev

#installing kf5coreaddons in ubuntu :-
sudo apt-get install libkf5coreaddons-dev

#installing kf5servive in ubuntu :-
sudo apt-get install -y libkf5service-dev

#installing libopencv in ubuntu :-
sudo apt-get install -y libopencv-dev

#installing libpthread in ubuntu :-
sudo apt-get install libpthread-stubs0-dev

#installing libpng in ubuntu :-
sudo apt-get install libpng-dev

#installing libtiff in ubuntu :-
sudo apt-get install -y libtiff-dev

#installing libjpeg in ubuntu :-
sudo apt-get install -y libjpeg-dev

#installing libboost in ubuntu :- 
sudo apt-get install libboost-all-dev

#installing liblcms in ubuntu :-
sudo apt-get install -y liblcms

#installing libexpat in ubuntu :-
sudo apt-get install -y libexpat-dev

#installing libexiv2 in ubuntu :-
sudo apt-get install libexiv2-dev
