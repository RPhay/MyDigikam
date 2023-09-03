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
          "qt6-base-private-dev"
          "qt6-svg-dev"
          "qt6-webengine-dev"
          "qt6-webview-dev"
          "qt6-networkauth-dev"
          "qt6-scxml-dev"
          "qt6-quick3d-dev"
          "qt6-quick3dphysics-dev"
          "qml6-module-qtquick3d-spatialaudio"
          "qt6-l10n-tools"
          "qt6-tools-dev"
          "qt6-tools-private-dev"
          "qt6-wayland"
          "qt6-wayland-dev"
          "libxcb-icccm4-dev"
          "libxcb-xres0-dev"
          "libxcb-keysyms1-dev"
          "libxcb-xfixes0-dev"
          "libxfixes-dev"
          "wayland-protocols"
          "plasma-wayland-protocols"
          "libical-dev"
          "libcanberra-dev"
)

for pkg in ${packages[@]}; do
    sudo apt-get install -y ${pkg}
    echo "-------------------------------------------------------------------"
done
