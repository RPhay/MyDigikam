#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libfilters_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/filterstatusbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geolocationfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/textfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mimefilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colorlabelfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/picklabelfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ratingfilter.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamgui
add_library(gui_filters_obj OBJECT ${libfilters_SRCS})

target_compile_definitions(gui_filters_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
