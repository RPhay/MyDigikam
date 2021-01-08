#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libnotificationmanager_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dnotificationwrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dnotificationpopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dnotificationwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dnotificationwidget_p.cpp
)

if(APPLE)
    set(libnotificationmanager_SRCS
        ${libnotificationmanager_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/macnotification.mm
    )
endif()

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

if(KF5Notifications_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::Notifications,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamcore
add_library(core_notificationmanager_obj OBJECT ${libnotificationmanager_SRCS})

target_compile_definitions(core_notificationmanager_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
