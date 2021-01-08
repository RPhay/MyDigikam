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

# -------------------------------------------------

add_executable(framesencoder ${CMAKE_CURRENT_SOURCE_DIR}/framesencoder.cpp)
ecm_mark_nongui_executable(framesencoder)

target_link_libraries(framesencoder

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -------------------------------------------------

add_executable(qtavcodecs ${CMAKE_CURRENT_SOURCE_DIR}/qtavcodecs.cpp)
ecm_mark_nongui_executable(qtavcodecs)

target_link_libraries(qtavcodecs

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -------------------------------------------------

add_executable(player ${CMAKE_CURRENT_SOURCE_DIR}/player.cpp)

target_link_libraries(player

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -------------------------------------------------

add_executable(videothumbtest ${CMAKE_CURRENT_SOURCE_DIR}/videothumbtest.cpp)
ecm_mark_nongui_executable(videothumbtest)

target_link_libraries(videothumbtest
                      digikamcore

                      ${COMMON_TEST_LINK}
)
