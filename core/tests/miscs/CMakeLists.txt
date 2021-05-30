#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
)

set(browser_gui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/browser_gui.cpp)
add_executable(browser_gui ${browser_gui_SRCS})

target_link_libraries(browser_gui

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------

set(onlinecheck_gui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/onlinecheck_gui.cpp)
add_executable(onlinecheck_gui ${onlinecheck_gui_SRCS})

target_link_libraries(onlinecheck_gui

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------

set(onlinedownload_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/onlinedownload_cli.cpp)
add_executable(onlinedownload_cli ${onlinedownload_cli_SRCS})

target_link_libraries(onlinedownload_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------

set(solidhardware_gui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/solidhardware_gui.cpp)
add_executable(solidhardware_gui ${solidhardware_gui_SRCS})

target_link_libraries(solidhardware_gui

                      digikamcore

                      ${COMMON_TEST_LINK}
)
