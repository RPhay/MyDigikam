#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
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

set(recognize_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/recognize.cpp)
add_executable(recognize ${recognize_SRCS})

target_link_libraries(recognize

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(facerec_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/face_rec.cpp)
add_executable(face_rec ${facerec_SRCS})

target_link_libraries(face_rec

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

# NOTE:Port this tool to new API

#set(facecluster_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/face_cluster.cpp)
#add_executable(face_cluster ${facecluster_SRCS})

#target_link_libraries(face_cluster

#                      digikamcore
#                      digikamgui
#                      digikamfacesengine
#                      digikamdatabase

#                      ${COMMON_TEST_LINK}
#)

# -----------------------------------------------------------------------------

set(benchmark_recognition_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_recognition.cpp)
add_executable(benchmark_recognition ${benchmark_recognition_SRCS})

target_link_libraries(benchmark_recognition

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(gui_recognition_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui_recognition.cpp)
add_executable(gui_recognition ${gui_recognition_SRCS})

target_link_libraries(gui_recognition

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(traindb_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/traindb.cpp)
add_executable(traindb ${traindb_SRCS})

target_link_libraries(traindb

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)
