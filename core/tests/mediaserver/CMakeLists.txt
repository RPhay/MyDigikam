#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

add_executable(dmediaserver_cli ${CMAKE_CURRENT_SOURCE_DIR}/dmediaserver_cli.cpp)

target_link_libraries(dmediaserver_cli

                      mediaserverbackend
                      digikamcore

                      ${COMMON_TEST_LINK}
 )

########################################################################
# CLI test tool from Platinum SDK

add_executable(filemediaserver_cli
               ${CMAKE_SOURCE_DIR}/core/dplugins/generic/tools/mediaserver/upnpsdk/Platinum/Source/Tests/FileMediaServer/FileMediaServerTest.cpp
)

target_link_libraries(filemediaserver_cli

                      digikamcore
                      mediaserverbackend

                      ${COMMON_TEST_LINK}
)
