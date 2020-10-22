#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/tools

                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(sendbymailplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/imageresizethread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/imageresizejob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/mailsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/mailprocess.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/mailwizard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/mailintropage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/mailalbumspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/mailimagespage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/mailsettingspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/mailfinalpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/outlookbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/thunderbirdbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/evolutionbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/kmailbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sylpheedbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/netscapebinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/clawsmailbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/balsabinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sendbymailplugin.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    SendByMail
                           SOURCES ${sendbymailplugin_SRCS}
)
