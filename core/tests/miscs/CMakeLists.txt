#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
)

set(browser_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/browser.cpp)
add_executable(browser ${browser_SRCS})

target_link_libraries(browser

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------

set(onlinecheck_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/onlinecheck.cpp)
add_executable(onlinecheck ${onlinecheck_SRCS})

target_link_libraries(onlinecheck

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------

set(onlinedownload_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/onlinedownload.cpp)
add_executable(onlinedownload ${onlinedownload_SRCS})

target_link_libraries(onlinedownload

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------

set(solidhardware_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/solidhardware.cpp)
add_executable(solidhardware ${solidhardware_SRCS})

target_link_libraries(solidhardware

                      digikamcore

                      ${COMMON_TEST_LINK}
)
