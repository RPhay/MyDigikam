#!/bin/bash

# Configuration for script to install kf6.
#
# SPDX-FileCopyrightText: 2015-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

########################################################################

# NOTE: include common.sh header first.
CheckSystemReleaseID

# Absolute path where are downloaded all tarballs to compile.
DOWNLOAD_DIR="$PWD/d"

# Absolute path where are compiled all tarballs
BUILDING_DIR="$PWD/b"

# Absolute path where are installed all software components
INSTALL_DIR="/usr"

########################################################################

# KDE Plasma version.
# See official release here: https://download.kde.org/stable/plasma/
DK_KP_VERSION="5.27.7"

# KDE Application version.
# See official release here: https://download.kde.org/stable/release-service/
DK_KA_VERSION="23.08.0"

# KDE KF5 frameworks version.
# See official release here: https://download.kde.org/stable/frameworks/
DK_KDE_VERSION="5.108"

DK_QTVERSION="6.3.1"
