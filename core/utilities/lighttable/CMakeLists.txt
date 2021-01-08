#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(liblighttable_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/lighttablepreview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lighttableview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lighttablethumbbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lighttablewindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lighttablewindow_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lighttablewindow_setup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lighttablewindow_tools.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lighttablewindow_import.cpp
)

include_directories($<TARGET_PROPERTY:Qt5::PrintSupport,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Network,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Network,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
)

if(Gphoto2_FOUND)
    include_directories(${GPHOTO2_INCLUDE_DIRS})
endif()

# Used by digikamgui
add_library(gui_lighttable_obj OBJECT ${liblighttable_SRCS})

target_compile_definitions(gui_lighttable_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lighttablewindowui5.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/digikam)
