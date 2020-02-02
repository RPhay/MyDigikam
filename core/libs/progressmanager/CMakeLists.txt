#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libprogressmanager_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/dhistoryview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/dprogresswdg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/overlaywidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/progressview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/progressmanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/statusbarprogresswidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/statusprogressbar.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/workingwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/dworkingpixmap.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/dlogoaction.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(progressmanager_src OBJECT ${libprogressmanager_SRCS})
