#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libdialogs_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dprogressdlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dbusydlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/filesaveoptionsdlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/iccprofileinfodlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/imagedialog.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/infodlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/libsinfodlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/rawcameradlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dconfigdlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dconfigdlgmodels.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dconfigdlgview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dconfigdlgview_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dconfigdlgwidgets.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dmessagebox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dsplashscreen.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/webbrowserdlg.cpp
)

set(libdeletedialog_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dialogs/deletedialog.cpp
)

if(LensFun_FOUND)
    include_directories(${LENSFUN_INCLUDE_DIRS})
endif()

include_directories(
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_QWEBENGINE)
    include_directories($<TARGET_PROPERTY:Qt5::WebEngineWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
else()
    include_directories($<TARGET_PROPERTY:Qt5::WebKitWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

#used by digikamcore
add_library(digikamdialogscore_src OBJECT ${libdialogs_SRCS})

add_library(digikamdeletedialog_src OBJECT ${libdeletedialog_SRCS})
