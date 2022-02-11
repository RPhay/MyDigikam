# Script to build KF6 Frameworks for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(EXTPREFIX_frameworks "${EXTPREFIX}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kdesrc-buildrc.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/kdesrc-buildrc)

ExternalProject_Add(ext_kf6_frameworks

    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    # We will use the KDE build source helper git repository.

    GIT_REPOSITORY https://invent.kde.org/sdk/kdesrc-build.git

    # Check version installed and system dependencies. Install modules configuration.

    CONFIGURE_COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/kdesrc-buildrc <SOURCE_DIR>/                                          &&
                      cd <SOURCE_DIR>                                                                                      &&
                      <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --version       &&
                      <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --metadata-only &&
                      <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --pretend       &&
                      ${CMAKE_CURRENT_SOURCE_DIR}/kf6-create-manifest.sh ${EXTERNALS_DOWNLOAD_DIR}

    # Apply patches, compile and install modules.

    BUILD_COMMAND cd <DOWNLOAD_DIR>/kf6/knotifications && git reset --hard && ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/knotifications-drop-phonon.patch &&
                  cd <DOWNLOAD_DIR>/kf6/kio            && git reset --hard && ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/kio-drop-ktextwidgets.patch      &&
                  cd <DOWNLOAD_DIR>/kf6/knotifyconfig  && git reset --hard && ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/knotifyconfig-drop-phonon.patch  &&
                  cd <DOWNLOAD_DIR>/kf6/breeze-icons   && git reset --hard && ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/breeze-dropsvg-rccprefix.patch   &&
                  cd <SOURCE_DIR>                                                                                                                                  &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure extra-cmake-modules                       &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kconfig                                   &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure breeze-icons                              &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kcoreaddons                               &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kwindowsystem                             &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure solid                                     &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure threadweaver                              &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure karchive                                  &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kdbusaddons                               &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure ki18n                                     &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kcrash                                    &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kcodecs                                   &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kauth                                     &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kguiaddons                                &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kwidgetsaddons                            &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kitemviews                                &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kcompletion                               &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kconfigwidgets                            &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kiconthemes                               &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kservice                                  &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kglobalaccel                              &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kxmlgui                                   &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kbookmarks                                &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kimageformats                             &&
# NOT YET PORTED  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure libksane                                  &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kjobwidgets                               &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kio                                       &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure knotifications                            &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure knotifyconfig                             &&
# NOT YET PORTED  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure marble                                    &&
                  <SOURCE_DIR>/kdesrc-build --rc-file=<SOURCE_DIR>/kdesrc-buildrc --delete-my-settings --stop-on-failure kcalendarcore

    INSTALL_COMMAND ""

    UPDATE_COMMAND ""
    ALWAYS 0
)
