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
