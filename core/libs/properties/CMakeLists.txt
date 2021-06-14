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
    $<TARGET_PROPERTY:Qt5::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

set(libitemproperties_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/itempropertiessidebar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itempropertiestab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemselectionpropertiestab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itempropertiesmetadatatab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itempropertiescolorstab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itempropertiestxtlabel.cpp
)

if(${Marble_FOUND})
    set(libitemproperties_SRCS
        ${libitemproperties_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/geolocation/itempropertiesgpstab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geolocation/itemgpsmodelhelper.cpp
   )
endif()

# Used by digikamcore
add_library(core_itemproperties_obj OBJECT ${libitemproperties_SRCS})

target_compile_definitions(core_itemproperties_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

set(libitemversionssidebar_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/history/itempropertiesversionstab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/versionstreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/versionsoverlays.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/versionsdelegate.cpp
)

set(libitempropertiesdigikam_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/itempropertiessidebardb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/import/importitempropertiessidebar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/import/importitempropertiestab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/captions/itemdescedittab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/captions/disjointmetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/captions/captionedit.cpp
)


# Used by digikamgui
add_library(gui_itempropertiesdigikam_obj OBJECT ${libitemversionssidebar_SRCS} ${libitempropertiesdigikam_SRCS})

target_compile_definitions(gui_itempropertiesdigikam_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
