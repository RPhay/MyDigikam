#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(dmediaservertest_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dmediaserver_test.cpp
)

add_executable(dmediaservertest ${dmediaservertest_SRCS})

if(APPLE)
    set(_extra_deps /System/Library/Frameworks/AppKit.framework)
endif()

target_link_libraries(dmediaservertest
                      digikamcore
                      mediaserverbackend

                      Qt5::Core
                      Qt5::Gui
                      Qt5::Xml
                      Qt5::XmlPatterns
                      Qt5::Widgets
                      Qt5::Test
                      Qt5::Sql

                      KF5::XmlGui
                      KF5::I18n
                      KF5::ConfigCore
                      KF5::Service
                      KF5::Solid

                     ${_extra_deps}
)

########################################################################
# CLI test tool from Platinum SDK
# NOTE : disable due to unexported symbols from UPNP sdk
#
#set(filemediaserver_SRCS
#    ${CMAKE_CURRENT_SOURCE_DIR}/../upnpsdk/Platinum/Source/Tests/FileMediaServer/FileMediaServerTest.cpp
#)
#
#add_executable(filemediaserver ${filemediaserver_SRCS})
#
#target_link_libraries(filemediaserver
#                      digikamcore
#                      mediaserverbackend
#)
