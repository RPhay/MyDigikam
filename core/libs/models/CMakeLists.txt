#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# ---------------------------------------------------------------------

set(libgenericmodels_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/categorizeditemmodel.cpp
)

# Used by digikamcore
add_library(core_digikamgenericmodels_obj OBJECT ${libgenericmodels_SRCS})

target_compile_definitions(core_digikamgenericmodels_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# ---------------------------------------------------------------------

set(libalbummodels_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/itemalbummodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemalbumfiltermodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abstractalbummodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/albummodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/albumfiltermodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/albummodeldragdrophandler.cpp
)

# Used by digikamgui
add_library(gui_digikammodels_obj OBJECT ${libalbummodels_SRCS})

target_compile_definitions(gui_digikammodels_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
