#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(akonadiiface_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/akonadiiface.cpp)

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5AkonadiContact_FOUND)
    include_directories(
        $<TARGET_PROPERTY:KF5::AkonadiCore,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:KF5::Contacts,INTERFACE_INCLUDE_DIRECTORIES>

        $<TARGET_PROPERTY:KF5::CoreAddons,INTERFACE_INCLUDE_DIRECTORIES>
    )
endif()

add_library(core_akonadiiface_obj OBJECT ${akonadiiface_SRCS})

target_compile_definitions(core_akonadiiface_obj
                           PRIVATE
                           digikamcore_EXPORTS
)


add_library(akonadiiface STATIC $<TARGET_OBJECTS:core_akonadiiface_obj>)

target_link_libraries(akonadiiface PRIVATE Qt5::Core KF5::I18n)

if(KF5AkonadiContact_FOUND)
    target_include_directories(core_akonadiiface_obj PRIVATE $<TARGET_PROPERTY:KF5::AkonadiContact,INTERFACE_INCLUDE_DIRECTORIES>)
    target_link_libraries(akonadiiface PRIVATE KF5::AkonadiContact KF5::Contacts)
endif()
