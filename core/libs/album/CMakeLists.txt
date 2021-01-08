#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libalbum_SRCS

    ${CMAKE_CURRENT_SOURCE_DIR}/engine/album.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/albumhistory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/albummodificationhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/albumthumbnailloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/albumwatch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/albumparser.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/albumpropsedit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/albumselectors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/albumselectcombobox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/albumselectdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/albumselectwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/albumselecttabs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/abstractalbumtreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/abstractcountingalbumtreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/abstractcheckablealbumtreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/albumtreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/labelstreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/albumlabelssearchhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/tagtreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/searchtreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/datetreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/treeview/albumselectiontreeview.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_album.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_palbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_talbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_dalbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_falbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_salbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_database.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/albummanager_collection.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamgui
add_library(gui_digikamalbum_obj OBJECT ${libalbum_SRCS})

target_compile_definitions(gui_digikamalbum_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
