#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libsettings_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/settings/applicationsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/settings/applicationsettings_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/settings/applicationsettings_albums.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/settings/applicationsettings_database.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/settings/applicationsettings_iconview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/settings/applicationsettings_mime.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/settings/applicationsettings_miscs.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/settings/applicationsettings_tooltips.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(digikamsettings_src OBJECT ${libsettings_SRCS})
