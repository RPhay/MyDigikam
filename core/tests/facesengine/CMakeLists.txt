#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

kde_enable_exceptions()

# -----------------------------------------------------------------------------

set(detect_SRCS detect.cpp)
add_executable(detect ${detect_SRCS})

target_link_libraries(detect

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(recognize_SRCS recognize.cpp)
add_executable(recognize ${recognize_SRCS})

target_link_libraries(recognize

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(facerec_SRCS face_rec.cpp)
add_executable(face_rec ${facerec_SRCS})

target_link_libraries(face_rec

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(facecluster_SRCS face_cluster.cpp)
add_executable(face_cluster ${facecluster_SRCS})

target_link_libraries(face_cluster

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(align_SRCS align.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/facesengine/alignment/congealing/funnelreal.cpp
)
add_executable(align ${align_SRCS})

target_link_libraries(align

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(preprocess_SRCS preprocess.cpp)
add_executable(preprocess ${preprocess_SRCS})

target_link_libraries(preprocess

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(traindb_SRCS traindb.cpp)
add_executable(traindb ${traindb_SRCS})

target_link_libraries(traindb

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(facesenginedemo_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/demo/demofaceengine.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demo/demomainwindow.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demo/demofaceitem.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demo/demobutton.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demo/demomarquee.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demo/demofancyrect.cpp
)

qt5_wrap_ui(facesenginedemo_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/demo/demomainwindow.ui)
add_executable(facesenginedemo ${facesenginedemo_SRCS})

target_link_libraries(facesenginedemo

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)
