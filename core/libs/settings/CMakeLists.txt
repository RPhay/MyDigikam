#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libsettings_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/applicationsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/applicationsettings_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/applicationsettings_albums.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/applicationsettings_database.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/applicationsettings_iconview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/applicationsettings_mime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/applicationsettings_miscs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/applicationsettings_tooltips.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamgui
add_library(gui_digikamsettings_obj OBJECT ${libsettings_SRCS})

target_compile_definitions(gui_digikamsettings_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
