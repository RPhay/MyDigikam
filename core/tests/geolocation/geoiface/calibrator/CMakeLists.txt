#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
)

set(calibrator_sources_gui ${CMAKE_CURRENT_SOURCE_DIR}/calibrator.cpp)

add_executable(geoiface_calibrator_gui ${calibrator_sources_gui})

target_link_libraries(geoiface_calibrator_gui

                      digikamcore

                      ${COMMON_TEST_LINK}
)
