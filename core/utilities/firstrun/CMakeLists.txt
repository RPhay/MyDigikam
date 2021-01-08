#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libfirstrun_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/firstrundlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/migratefromdigikam4page.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/welcomepage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collectionpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/databasepage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rawpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tooltipspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/previewpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openfilepage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadatapage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/startscanpage.cpp
)

include_directories($<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamgui
add_library(gui_firstrun_obj OBJECT ${libfirstrun_SRCS})

target_compile_definitions(gui_firstrun_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
