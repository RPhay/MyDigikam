#!/bin/bash

# Configuration for script to build AppImage bundle.
#
# SPDX-FileCopyrightText: 2015-2024 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

########################################################################

# NOTE: include common.sh header first.
CheckSystemReleaseID

# digiKam tag version from git. Official tarball do not include extra shared libraries.
# The list of tags can be listed with this url: https://invent.kde.org/graphics/digikam/-/tags
# If you want to package current implementation from git, use "master" as tag.
#DK_VERSION=v7.0.0
DK_VERSION=master
#DK_VERSION=gsoc21-qt6-port

# Absolute path where are installed all software components
INSTALL_DIR="/usr"

# Absolute path where are bundled all software components
APP_IMG_DIR="/digikam.appdir"

########################################################################

# URL to git repository to checkout digiKam source code
# git protocol version which require a developer account with ssh keys.
DK_GITURL="git@invent.kde.org:graphics/digikam.git"
# https protocol version which give annonyous access.
#DK_GITURL="https://invent.kde.org/graphics/digikam.git"

# Location to build source code.
DK_BUILDTEMP=$BUILDING_DIR/dktemp

# Qt version to use in bundle. Possible values:
# - 5:    stable Qt5 release.
# - 6:    stable Qt6 release.

#DK_QTVERSION="5"
DK_QTVERSION="6"

# Installer will include or not digiKam debug symbols
DK_DEBUG=0

# Option to use QtWebEngine instead QtWebkit
DK_QTWEBENGINE=1

# Installer sub version to differentiates newer updates of the installer itself, even if the underlying application hasn’t changed.
#DK_SUBVER="-01"

# Sign bundles with GPG. Passphrase must be hosted in ~/.gnupg/dkorg-gpg-pwd.txt
DK_SIGN=0

# Upload automatically bundle to files.kde.org (pre-release only).
DK_UPLOAD=1
DK_UPLOADURL="digikam@tinami.kde.org"
DK_UPLOADDIR="/srv/archives/files/digikam/"

# KDE frameworks version + Upload URL.
# See official release here: https://download.kde.org/stable/frameworks/

if [[ $DK_QTVERSION == 5 ]] ; then

    # Absolute path where are downloaded all tarballs to compile.
    DOWNLOAD_DIR="/d"

    # Absolute path where are compiled all tarballs
    BUILDING_DIR="/b/$DK_VERSION"

    # KDE Plasma version.
    # See official release here: https://download.kde.org/stable/plasma/
    DK_KDE_VERSION="5.116"

    # KDE Application version.
    # See official release here: https://download.kde.org/stable/release-service/
    DK_KP_VERSION="5.27.11"

    # KDE KF6 frameworks version.
    # See official release here: https://download.kde.org/stable/frameworks/
    DK_KA_VERSION="24.05.1"

else

    # KDE Plasma version.
    # See official release here: https://download.kde.org/stable/plasma/
    DK_KP_VERSION="v6.0.4"

    # KDE Application version.
    # See official release here: https://download.kde.org/stable/release-service/
    DK_KA_VERSION="v24.02.2"

    # KDE KF6 frameworks version.
    # See official release here: https://download.kde.org/stable/frameworks/
    DK_KDE_VERSION="v6.2.0"

    # Absolute path where are downloaded all tarballs to compile.
    DOWNLOAD_DIR="/mnt/data/d"

    # Absolute path where are compiled all tarballs
    BUILDING_DIR="/mnt/data/b/$DK_VERSION"

fi

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
ext_plasma-wayland-protocols \
ext_knotifications \
ext_kjobwidgets \
ext_kio \
ext_knotifyconfig \
ext_sonnet \
ext_ktextwidgets \
ext_qca \
ext_kwallet \
ext_ksanecore \
ext_libksane \
ext_kcalendarcore \
"

#ext_kfilemetadata \
#ext_kdoctools \
#ext_phonon \
#ext_qca \
#ext_kpackage \
#ext_attica \
#ext_knewstuff \
#ext_kitemmodels \
#ext_kparts \
#ext_krunner \
