# Script to build KF6 Frameworks for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(EXTPREFIX_frameworks "${EXTPREFIX}")

ExternalProject_Add(ext_kf6_frameworks
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://invent.kde.org/sdk/kdesrc-build.git

    # Check version installed and system dependencies. Install modules configuration.

    CONFIGURE_COMMAND cd <SOURCE_DIR> &&
                      ./kdesrc-build --rc-file=${CMAKE_CURRENT_SOURCE_DIR}/kdesrc-buildrc --version &&
                      ./kdesrc-build --rc-file=${CMAKE_CURRENT_SOURCE_DIR}/kdesrc-buildrc --metadata-only &&
                      ./kdesrc-build --rc-file=${CMAKE_CURRENT_SOURCE_DIR}/kdesrc-buildrc --pretend

    BUILD_COMMAND cd <SOURCE_DIR> &&
                  kdesrc-build --rc-file=${CMAKE_CURRENT_SOURCE_DIR}/kdesrc-buildrc --stop-on-failure
                               extra-cmake-modules
                               kconfig
                               breeze-icons
                               solid
                               kcoreaddons
                               threadweaver
                               kwindowsystem
                               karchive
                               kdbusaddons
                               ki18n
                               kcrash
                               kcodecs
                               kauth
                               kguiaddons
                               kwidgetsaddons
                               kitemviews
                               kcompletion
                               kconfigwidgets
                               kiconthemes
                               kservice
                               kglobalaccel
                               kcalendarcore
                               kxmlgui
                               kbookmarks
                               kimageformats
                               knotifyconfig
                               knotifications
                               kjobwidgets
                               sonnet
                               ktextwidgets
                               kio
                               kinit

    INSTALL_COMMAND ""

    UPDATE_COMMAND ""
    ALWAYS 0
)
