#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

kde_enable_exceptions()

add_custom_target(datalink_rec ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/../data ${CMAKE_CURRENT_BINARY_DIR}/data)

add_custom_target(scriptlink_rec ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/../scripts ${CMAKE_CURRENT_BINARY_DIR}/scripts)

# -----------------------------------------------------------------------------

set(recognize_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/recognize_cli.cpp)
add_executable(recognize_cli ${recognize_cli_SRCS})

target_link_libraries(recognize_cli

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(facerec_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/face_rec_cli.cpp)
add_executable(face_rec_cli ${facerec_cli_SRCS})

target_link_libraries(face_rec_cli

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

# NOTE:Port this tool to new API

#set(facecluster_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/face_cluster_cli.cpp)
#add_executable(face_cluster_cli ${facecluster_cli_SRCS})

#target_link_libraries(face_cluster_cli

#                      digikamcore
#                      digikamgui
#                      digikamfacesengine
#                      digikamdatabase

#                      ${COMMON_TEST_LINK}
#)

# -----------------------------------------------------------------------------

set(benchmark_recognition_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_recognition_cli.cpp)
add_executable(benchmark_recognition_cli ${benchmark_recognition_cli_SRCS})

target_link_libraries(benchmark_recognition_cli

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(recognition_gui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/recognition_gui.cpp)
add_executable(recognition_gui ${recognition_gui_SRCS})

target_link_libraries(recognition_gui

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(traindb_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/traindb_cli.cpp)
add_executable(traindb_cli ${traindb_cli_SRCS})

target_link_libraries(traindb_cli

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)
