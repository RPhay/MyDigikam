#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libtags_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/addtagscombobox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/addtagslineedit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tagcheckview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tagfolderview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tagslineeditoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tagspopupmenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tageditdlg.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/engine/tagscompleter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/tagmodificationhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/tagsactionmngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/taggingaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/taggingactionfactory.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/manager/tagsmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/tagpropwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/taglist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/tagmngrtreeview.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/manager/models/tagmngrlistview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/models/tagmngrlistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/models/tagmngrlistitem.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5IconThemes_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::IconThemes,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamgui
add_library(gui_digikamtags_obj OBJECT ${libtags_SRCS})

target_compile_definitions(gui_digikamtags_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
