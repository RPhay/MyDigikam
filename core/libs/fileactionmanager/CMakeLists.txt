#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(fileactionmanager_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/metadatahub.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadatahubmngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadatastatusbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fileactionprogress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fileactionmngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fileactionmngr_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iteminfotasksplitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/databaseworkeriface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fileworkeriface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fileactionimageinfolist.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamgui
add_library(gui_fileactionmanager_obj OBJECT ${fileactionmanager_SRCS})

target_compile_definitions(gui_fileactionmanager_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
