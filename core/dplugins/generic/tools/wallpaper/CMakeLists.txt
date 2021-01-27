#
# Copyright (C) 2019      by Igor Antropov <antropovi at yahoo dot com>
# Copyright (c) 2019-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}

                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(wallpaperplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/wallpaperplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wallpaperplugindlg.cpp
)

if(WIN32)

    set(wallpaperplugin_SRCS ${wallpaperplugin_SRCS}
                             ${CMAKE_CURRENT_SOURCE_DIR}/wallpaperplugin_windows.cpp)

    set(compile_wallpaperplugin ON)

elseif(APPLE)

    set(wallpaperplugin_SRCS ${wallpaperplugin_SRCS}
                             ${CMAKE_CURRENT_SOURCE_DIR}/wallpaperplugin_macos.cpp)

    set(compile_wallpaperplugin ON)

elseif(ENABLE_DBUS)

    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)

    set(wallpaperplugin_SRCS ${wallpaperplugin_SRCS}
                             ${CMAKE_CURRENT_SOURCE_DIR}/wallpaperplugin_dbus.cpp)

    set(compile_wallpaperplugin ON)

endif()

if(compile_wallpaperplugin)

    message(STATUS "WallPaper plugin will be compiled......... YES")

    DIGIKAM_ADD_GENERIC_PLUGIN(NAME    Wallpaper
                               SOURCES ${wallpaperplugin_SRCS}
    )

else()

    message(STATUS "WallPaper plugin will be compiled......... NO")

endif()
