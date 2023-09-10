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

# KDE Plasma git tag version.
DK_KP_VERSION="5.27.7"

# KDE Application git tag version.
DK_KA_VERSION="23.08.0"

# KDE KF6 frameworks git tag version.
DK_KDE_VERSION="master"

# NOTE: The order to compile each component here is very important.

FRAMEWORK_COMPONENTS="\
ext_extra-cmake-modules \
ext_kconfig \
ext_breeze-icons \
ext_kcoreaddons \
ext_kwindowsystem \
ext_solid \
ext_threadweaver \
ext_karchive \
ext_kdbusaddons \
ext_ki18n \
ext_kcrash \
ext_kcodecs \
ext_kauth \
ext_kguiaddons \
ext_kwidgetsaddons \
ext_kitemviews \
ext_kcompletion \
ext_kcolorscheme \
ext_kconfigwidgets \
ext_kiconthemes \
ext_kservice \
ext_kglobalaccel \
ext_kxmlgui \
ext_kbookmarks \
ext_kimageformats \
ext_sonnet \
ext_ktextwidgets \
ext_ksanecore \
ext_libksane \
ext_knotifications \
ext_kjobwidgets \
ext_kio \
ext_knotifyconfig \
ext_kfilemetadata \
ext_kcalendarcore \
"

#ext_marble \
