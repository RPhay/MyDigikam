#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

set(libdragdrop_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/abstractitemdragdrophandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dragdropimplementations.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(core_dragdrop_obj OBJECT ${libdragdrop_SRCS})

target_compile_definitions(core_dragdrop_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
