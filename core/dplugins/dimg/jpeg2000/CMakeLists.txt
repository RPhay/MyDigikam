#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

set(dimgjpeg2000plugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgjpeg2000plugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgjpeg2000loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgjpeg2000loader_load.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgjpeg2000loader_save.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dimgjpeg2000exportsettings.cpp
)

DIGIKAM_ADD_DIMG_PLUGIN(NAME    JPEG2000
                        SOURCES ${dimgjpeg2000plugin_SRCS}
                        DEPENDS ${JASPER_LIBRARIES}
)
