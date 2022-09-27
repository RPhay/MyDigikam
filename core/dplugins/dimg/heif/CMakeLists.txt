#
# SPDX-FileCopyrightText: 2019-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(dimgheifplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifloader_load.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifloader_save.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgheifexportsettings.cpp
)

DIGIKAM_ADD_DIMG_PLUGIN(NAME    HEIF
                        SOURCES ${dimgheifplugin_SRCS}
                        DEPENDS ${X265_LIBRARIES}
                                ${CMAKE_THREAD_LIBS_INIT}
                                Libheif::Libheif
)
