#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories(
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::TextWidgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::SonnetUi,INTERFACE_INCLUDE_DIRECTORIES>
)


set(textconverterplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ocroptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/textconverterplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/textconverterdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/textconverterlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/textconvertersettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ocrtesseractengine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/textconvertertask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/textconverterthread.cpp
)


# For Unit-tests and plugin
add_library(textconverterbackend STATIC ${textconverterplugin_SRCS})


target_link_libraries(textconverterbackend
                      PRIVATE
                      digikamcore
                      KF5::TextWidgets
                      KF5::SonnetUi
)


DIGIKAM_ADD_GENERIC_PLUGIN(NAME    TEXTConverter
                          SOURCES ${textconverterplugin_SRCS}
                          DEPENDS textconverterbackend
)
