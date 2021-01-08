#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libtemplate_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/templatemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/templatelist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/templateselector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/templateviewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/templatepanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subjectedit.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamgui
add_library(gui_digikamtemplate_obj OBJECT  ${libtemplate_SRCS})

target_compile_definitions(gui_digikamtemplate_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
