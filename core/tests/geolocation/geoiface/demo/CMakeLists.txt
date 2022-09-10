#
# SPDX-FileCopyrightText: 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

set(geoiface_demo_gui_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/demo-main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/myimageitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mytreewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mydragdrophandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
)

add_executable(geoiface_demo_gui ${geoiface_demo_gui_SRCS})

target_link_libraries(geoiface_demo_gui

                      digikamcore

                      ${COMMON_TEST_LINK}
)
