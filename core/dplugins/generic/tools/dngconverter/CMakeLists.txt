#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(dngconverterplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dngconverterplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngconverterlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngconvertertask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngconverterthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngconverterdialog.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    DNGConverter
                           SOURCES ${dngconverterplugin_SRCS}
)
