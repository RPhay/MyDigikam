#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libitemproperties_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/properties/itempropertiessidebar.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/itempropertiestab.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/itempropertiesmetadatatab.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/itempropertiescolorstab.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/itempropertiestxtlabel.cpp
)

set(libitemversionssidebar_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/properties/history/itempropertiesversionstab.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/history/versionstreeview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/history/versionsoverlays.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/history/versionsdelegate.cpp
)

if(${Marble_FOUND})
    set(libitemproperties_SRCS
        ${libitemproperties_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/properties/geolocation/itempropertiesgpstab.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/properties/geolocation/itemgpsmodelhelper.cpp
   )
endif()

set(libitempropertiesdigikam_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/properties/itempropertiessidebardb.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/import/importitempropertiessidebar.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/import/importitempropertiestab.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/captions/itemdescedittab.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/captions/disjointmetadata.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/properties/captions/captionedit.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# used to build digikamcore
add_library(itemproperties_src OBJECT ${libitemproperties_SRCS})

add_library(itempropertiesdigikam_src OBJECT ${libitemversionssidebar_SRCS} ${libitempropertiesdigikam_SRCS})
