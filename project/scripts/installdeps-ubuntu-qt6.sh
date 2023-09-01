#!/bin/bash

# Script to install dependencies under Ubuntu Linux to compile digiKam with Qt6.
# This script must be run as sudo
# *Ubuntu* compatible version >= 18.04
#
# SPDX-FileCopyrightText: 2021-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if [[ $EUID -ne 0 ]]; then
    echo "This script should be run as root using sudo command."
    exit 1
fi

packages=("qt6-base-dev"
          "qt6-base-dev-tools"
          "qt6-svg-dev"
          "qt6-websockets-dev"
          "qt6-serialport-dev"
          "qt6-multimedia-dev"
          "qt6-5compat-dev"
)

for pkg in ${packages[@]}; do
    sudo apt-get install -y ${pkg}
    echo "-------------------------------------------------------------------"
done
