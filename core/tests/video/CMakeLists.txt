#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
)

if (Qt6_FOUND)
  include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core5Compat,INTERFACE_INCLUDE_DIRECTORIES>)
endif ()

# -------------------------------------------------

add_executable(framesencoder_cli ${CMAKE_CURRENT_SOURCE_DIR}/framesencoder_cli.cpp)
ecm_mark_nongui_executable(framesencoder_cli)

target_link_libraries(framesencoder_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -------------------------------------------------

add_executable(qtavcodecs_cli ${CMAKE_CURRENT_SOURCE_DIR}/qtavcodecs_cli.cpp)
ecm_mark_nongui_executable(qtavcodecs_cli)

target_link_libraries(qtavcodecs_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -------------------------------------------------

add_executable(player_gui ${CMAKE_CURRENT_SOURCE_DIR}/player_gui.cpp)

target_link_libraries(player_gui

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -------------------------------------------------

add_executable(videothumb_cli ${CMAKE_CURRENT_SOURCE_DIR}/videothumb_cli.cpp)
ecm_mark_nongui_executable(videothumb_cli)

target_link_libraries(videothumb_cli
                      digikamcore

                      ${COMMON_TEST_LINK}
)
