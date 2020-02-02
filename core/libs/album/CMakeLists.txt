#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libalbum_SRCS

    ${CMAKE_SOURCE_DIR}/core/libs/album/engine/album.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/engine/albumhistory.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/engine/albummodificationhelper.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/engine/albumthumbnailloader.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/engine/albumwatch.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/album/widgets/albumpropsedit.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/widgets/albumselectors.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/widgets/albumselectcombobox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/widgets/albumselectdialog.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/widgets/albumselectwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/widgets/albumselecttabs.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/abstractalbumtreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/abstractcountingalbumtreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/abstractcheckablealbumtreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/albumtreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/labelstreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/albumlabelssearchhandler.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/tagtreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/searchtreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/datetreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/treeview/albumselectiontreeview.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_album.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_palbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_talbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_dalbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_falbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_salbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_database.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/album/manager/albummanager_collection.cpp
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

add_library(digikamalbum_src OBJECT ${libalbum_SRCS})
