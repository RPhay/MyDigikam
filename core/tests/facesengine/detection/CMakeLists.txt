#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

kde_enable_exceptions()

add_custom_target(datalink_det ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/../data ${CMAKE_CURRENT_BINARY_DIR}/data)

add_custom_target(scriptlink_det ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/../scripts ${CMAKE_CURRENT_BINARY_DIR}/scripts)

# -----------------------------------------------------------------------------

set(detect_gui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/detect_gui.cpp)
add_executable(detect_gui ${detect_gui_SRCS})

target_link_libraries(detect_gui

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(dnndetection_gui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_dnndetection_gui.cpp)
add_executable(benchmark_dnndetection_gui ${dnndetection_gui_SRCS})

target_link_libraries(benchmark_dnndetection_gui

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

