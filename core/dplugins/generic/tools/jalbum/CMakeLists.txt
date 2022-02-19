#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/
                    ${CMAKE_CURRENT_SOURCE_DIR}/tools

                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

set(jalbumplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/jalbumsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/jalbumgenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/jalbumintropage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/jalbumfinalpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/jalbumoutputpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/jalbumselectionpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/jalbumwizard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/jalbumjar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/jalbumjava.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jalbumplugin.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    JAlbum
                           SOURCES ${jalbumplugin_SRCS}
)
