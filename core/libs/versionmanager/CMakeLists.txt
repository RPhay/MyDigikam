#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libversionmanager_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/versionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/versionmanagersettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/versionnamingscheme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/versionfileoperation.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(core_versionmanager_obj OBJECT ${libversionmanager_SRCS})

target_compile_definitions(core_versionmanager_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
