#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
)

set(multicorerawtopng_gui_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processordlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rawtopngconverterthread.cpp
)

add_executable(multicorerawtopng_gui ${multicorerawtopng_gui_SRCS})

target_link_libraries(multicorerawtopng_gui

                      digikamcore

                      ${COMMON_TEST_LINK}
)
