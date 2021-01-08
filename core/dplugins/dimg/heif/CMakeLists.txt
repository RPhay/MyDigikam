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

set(dimgheifplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifloader_load.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifloader_save.cpp
)

DIGIKAM_ADD_DIMG_PLUGIN(NAME    HEIF
                        SOURCES ${dimgheifplugin_SRCS}
                        DEPENDS ${X265_LIBRARIES}
                                ${CMAKE_THREAD_LIBS_INIT}
)
