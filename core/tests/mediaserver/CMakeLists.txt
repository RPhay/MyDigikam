#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(dmediaservertest_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dmediaserver_test.cpp
)

add_executable(dmediaservertest ${dmediaservertest_SRCS})

target_link_libraries(dmediaservertest

                      mediaserverbackend
                      digikamcore

                      ${COMMON_TEST_LINK}
 )

########################################################################
# CLI test tool from Platinum SDK

set(filemediaserver_SRCS
    ${CMAKE_SOURCE_DIR}/core/dplugins/generic/tools/mediaserver/upnpsdk/Platinum/Source/Tests/FileMediaServer/FileMediaServerTest.cpp
)

add_executable(filemediaserver ${filemediaserver_SRCS})

target_link_libraries(filemediaserver
                      digikamcore
                      mediaserverbackend

                      ${COMMON_TEST_LINK}
)
