#
# SPDX-FileCopyrightText: 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Network,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

                    ${CMAKE_CURRENT_SOURCE_DIR}/dialog
                    ${CMAKE_CURRENT_SOURCE_DIR}/kmlexport
                    ${CMAKE_CURRENT_SOURCE_DIR}/searches
)

set(geolocationeditplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/searches/searchbackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/searches/searchresultmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/searches/searchresultmodelhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/searches/searchwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialog/gpsitemdetails.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialog/geolocationedit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kmlexport/geodataparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kmlexport/kmlgpsdataparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kmlexport/kmlwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kmlexport/kmlexport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geolocationeditplugin.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    GeolocationEdit
                           SOURCES ${geolocationeditplugin_SRCS}
                           DEPENDS Qt${QT_VERSION_MAJOR}::Concurrent
)
