#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libfuzzysearch_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/fuzzysearchview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fuzzysearchview_sketch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fuzzysearchview_similar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/findduplicatesview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/findduplicatesalbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/findduplicatesalbumitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sketchwidget.cpp
)

include_directories($<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamgui
add_library(gui_fuzzysearch_obj OBJECT ${libfuzzysearch_SRCS})

target_compile_definitions(gui_fuzzysearch_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
