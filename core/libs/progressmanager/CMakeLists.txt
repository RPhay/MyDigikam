#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libprogressmanager_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dhistoryview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dprogresswdg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/overlaywidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/progressview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/progressmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/statusbarprogresswidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/statusprogressbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/workingwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dworkingpixmap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dlogoaction.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(core_progressmanager_obj OBJECT ${libprogressmanager_SRCS})

target_compile_definitions(core_progressmanager_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
