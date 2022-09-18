#
# SPDX-FileCopyrightText: 2010-2022, Gilles Caulier, <caulier dot gilles at gmail dot com>
# SPDX-FileCopyrightText: 2020     , Minh Nghia Duong <minhnghiaduong997 at gmail dot com>

# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dialogs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets)

set(slideshowplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/common/slideshowloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/slideshowsettings.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/slidetoolbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/slideproperties.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/slideend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/slideerror.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/slideimage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/slideosd.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/setupslideshow_dialog.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/slideshowplugin.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    SlideShow
                           SOURCES ${slideshowplugin_SRCS}
)
