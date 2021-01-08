#
# Copyright (c) 2015-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(rawimportnativeplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/rawimportnativeplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rawimport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rawpreview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rawsettingsbox.cpp
)

DIGIKAM_ADD_RAWIMPORT_PLUGIN(NAME    Native
                             SOURCES ${rawimportnativeplugin_SRCS}
)
