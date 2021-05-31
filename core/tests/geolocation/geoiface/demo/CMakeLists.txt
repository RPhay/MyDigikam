#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>

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
