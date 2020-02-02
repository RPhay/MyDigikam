#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libtags_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/tags/widgets/addtagscombobox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/widgets/addtagslineedit.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/widgets/tagcheckview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/widgets/tagfolderview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/widgets/tagslineeditoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/widgets/tagspopupmenu.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/widgets/tageditdlg.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/tags/engine/tagscompleter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/engine/tagmodificationhelper.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/engine/tagsactionmngr.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/engine/taggingaction.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/engine/taggingactionfactory.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/tags/manager/tagsmanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/manager/tagpropwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/manager/taglist.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/manager/tagmngrtreeview.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/tags/manager/models/tagmngrlistview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/manager/models/tagmngrlistmodel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/tags/manager/models/tagmngrlistitem.cpp
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

add_library(digikamtags_src OBJECT ${libtags_SRCS})
