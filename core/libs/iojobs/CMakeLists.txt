#
# Copyright (c) 2015 by Mohamed_Anwer, <m_dot_anwer at gmx dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libiojobs_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/iojob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iojobdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iojobsthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iojobsmanager.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if (Qt6_FOUND)
  include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core5Compat,INTERFACE_INCLUDE_DIRECTORIES>)
endif ()

# Used by digikamgui
add_library(gui_iojobs_obj OBJECT ${libiojobs_SRCS})

target_compile_definitions(gui_iojobs_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
