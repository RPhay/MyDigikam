# Script to build KF6 Frameworks for digiKam bundle.
#
# Copyright (c) 2015-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

project(install-kf6)

cmake_minimum_required(VERSION 3.3.2)

function(JoinListAsString VALUES GLUE OUTPUT)
    string(REPLACE ";" "${GLUE}" _TMP_STR "${VALUES}")
    set(${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endfunction()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Compiling in the source directory is not supported. Use for example 'mkdir build; cd build; cmake ..'.")
endif()

# Tools must be obtained to work with:
include (ExternalProject)

# allow specification of a directory with pre-downloaded
# requirements
if(NOT IS_DIRECTORY ${EXTERNALS_DOWNLOAD_DIR})
    message(FATAL_ERROR "No externals download dir set. Use -DEXTERNALS_DOWNLOAD_DIR")
endif()

if(NOT IS_DIRECTORY ${INSTALL_ROOT})
    message(FATAL_ERROR "No install dir set. Use -DINSTALL_ROOT")
endif()

set(TOP_INST_DIR ${INSTALL_ROOT})
set(EXTPREFIX "${TOP_INST_DIR}")
set(CMAKE_PREFIX_PATH "${EXTPREFIX}")

message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_CL_64:     ${CMAKE_CL_64}")

set(GLOBAL_BUILD_TYPE RelWithDebInfo)
set(GLOBAL_PROFILE ${GLOBAL_PROFILE} -DBUILD_TESTING=false)
set(PATCH_COMMAND patch)

set(EXTPREFIX_frameworks "${EXTPREFIX}")

function(InstallKF6Component
         name
         version
         argoptions
         patch)

        if(${name} STREQUAL "libksane")
            set(giturl "https://invent.kde.org/graphics/${name}.git")
        elseif(${name} STREQUAL "ksanecore")
            set(giturl "https://invent.kde.org/libraries/${name}.git")
        else()
            set(giturl "https://invent.kde.org/frameworks/${name}.git")
        endif()

        message(STATUS "--------------------------------")
        message(STATUS "KDE component    : ${name}")
        message(STATUS "Component version: ${version}")
        message(STATUS "Component Git Url: ${giturl}")
        message(STATUS "Configure options: ${argoptions}")
        message(STATUS "Patch to apply   : ${patch}")

        if(NOT "${patch}" STREQUAL "")
            set(patch "${PATCH_COMMAND};-p1;-i;${CMAKE_CURRENT_SOURCE_DIR}/${patch}")
        endif()

        ExternalProject_Add(ext_${name}
                            DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
                            GIT_REPOSITORY ${giturl}

                            PATCH_COMMAND ${patch}

                            UPDATE_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/fix-qt-version.sh;<SOURCE_DIR>"

                            INSTALL_DIR ${EXTPREFIX_frameworks}
                            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_frameworks}
                                       -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                                       ${GLOBAL_PROFILE}
                                       -DCMAKE_SYSTEM_PREFIX_PATH=${EXTPREFIX}
                                       -DBUILD_TESTING=OFF
                                       "${argoptions}"
                                       -Wno-dev
                            UPDATE_COMMAND ""
                            ALWAYS 0
        )

endfunction()

set(BreezeIconsOptions -DBINARY_ICONS_RESOURCE=ON)
set(Qt6CommonOptions -DQT_MAJOR_VERSION=6 -DBUILD_WITH_QT6=ON)

InstallKF6Component(extra-cmake-modules ${KDE_VERSION} "${Qt6CommonOptions}"                                    "ecm-qml-policy.patch")
InstallKF6Component(kconfig             ${KDE_VERSION} "${Qt6CommonOptions};-DKCONFIG_USE_QML=OFF"              "")
InstallKF6Component(breeze-icons        ${KDE_VERSION} "${Qt6CommonOptions};${BreezeIconsOptions}"              "breeze-dropsvg-rccprefix.patch")
InstallKF6Component(solid               ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kcoreaddons         ${KDE_VERSION} "${Qt6CommonOptions};-DKCOREADDONS_USE_QML=OFF"          "")
InstallKF6Component(threadweaver        ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kwindowsystem       ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(karchive            ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kdbusaddons         ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(ki18n               ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kcrash              ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kcodecs             ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kauth               ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kguiaddons          ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kwidgetsaddons      ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kitemviews          ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kcompletion         ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kcolorscheme        ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kconfigwidgets      ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kiconthemes         ${KDE_VERSION} "${Qt6CommonOptions};-DKICONTHEMES_USE_QTQUICK=OFF"      "")
InstallKF6Component(kservice            ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kglobalaccel        ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kcalendarcore       ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kxmlgui             ${KDE_VERSION} "${Qt6CommonOptions}"                                    "kxmlgui-qt-6.4.patch")
InstallKF6Component(kbookmarks          ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kimageformats       ${KDE_VERSION} "${Qt6CommonOptions}"                                    "kimageformat-qt-6.4.patch")
InstallKF6Component(knotifyconfig       ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(knotifications      ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kjobwidgets         ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(sonnet              ${KDE_VERSION} "${Qt6CommonOptions};-DSONNET_USE_QML=OFF"               "")
InstallKF6Component(ktextwidgets        ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kio                 ${KDE_VERSION} "${Qt6CommonOptions}"                                    "kio-drop-deps.patch")
InstallKF6Component(kinit               ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kdoctools           ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(ksanecore           ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(libksane            ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
InstallKF6Component(kfilemetadata       ${KDE_VERSION} "${Qt6CommonOptions}"                                    "")
