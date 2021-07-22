#
# Copyright (c) 2021 by Anjani Kumar, <anjanik012 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libnowebwidget_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/dnowebwidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dnowebdlg.cpp
        )

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

        $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
        )


if (Qt6_FOUND)
  include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core5Compat,INTERFACE_INCLUDE_DIRECTORIES>)
endif ()
add_library(gui_nowebwidget_obj OBJECT ${libnowebwidget_SRCS})

target_compile_definitions(gui_nowebwidget_obj
        PRIVATE
        digikamgui_EXPORTS
        )
