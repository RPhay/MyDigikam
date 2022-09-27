#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.3.2)

if(POLICY CMP0063)
    # C++ symbols visibility policy introduced in CMake version 3.3
    # Details: https://cmake.org/cmake/help/git-stage/policy/CMP0063.html
    cmake_policy(SET CMP0063 NEW)
endif()

include (ExternalProject)

ExternalProject_Add(icon-rcc
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

    GIT_REPOSITORY https://invent.kde.org/frameworks/breeze-icons.git
    GIT_TAG v${KF5_VERSION}.0

    PATCH_COMMAND patch -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/breeze-mxe-linux.patch

    CMAKE_ARGS -DBINARY_ICONS_RESOURCE=ON -DSKIP_INSTALL_ICONS=ON -DQT_MAJOR_VERSION=5

    BUILD_IN_SOURCE 0
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
    ALWAYS 0
)
