#
# Copyright (c) 2015-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::PrintSupport,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(printplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/printhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/printoptionspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/printconfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/printplugin.cpp
)

ki18n_wrap_ui(printplugin_SRCS
              ${CMAKE_CURRENT_SOURCE_DIR}/printoptionspage.ui
)

DIGIKAM_ADD_EDITOR_PLUGIN(NAME    PrintTool
                          SOURCES ${printplugin_SRCS}
                          DEPENDS Qt${QT_VERSION_MAJOR}::PrintSupport
)
