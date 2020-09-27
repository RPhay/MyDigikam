#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# ---------------------------------------------------------------------------

kde_enable_exceptions()

include_directories($<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

                    ${CMAKE_CURRENT_SOURCE_DIR}/common
                    ${CMAKE_CURRENT_SOURCE_DIR}/facedb

                    ${CMAKE_CURRENT_SOURCE_DIR}/detection
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn

                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing
                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor
                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/recognition

                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn
)

# ---------------------------------------------------------------------------

set(facesengine_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/common/identity.cpp

                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/facepreprocessor.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/recognition/recognitionpreprocessor.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/recognition/openfacepreprocessor.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/matrixoperations.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/pointtransformaffine.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/qdatastreamoverloads.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/fullobjectdetection.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/shapepredictor.cpp

                         ${CMAKE_CURRENT_SOURCE_DIR}/detection/facedetector.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn/opencvdnnfacedetector.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn/dnnfacedetectorssd.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn/dnnfacedetectoryolo.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn/dnnfacedetectorbase.cpp
)

# Used by digikamcore
add_library(core_digikamfacesengine_obj
            OBJECT
            ${facesengine_LIB_SRCS}
)

target_compile_definitions(core_digikamfacesengine_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# For unit-tests
add_library(digikamfacesengine STATIC $<TARGET_OBJECTS:core_digikamfacesengine_obj>)

# ---------------------------------------------------------------------------

set(facesengine_database_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiontrainingprovider.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/dataproviders.cpp

                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facialrecognition_wrapper.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facialrecognition_wrapper_p.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facialrecognition_wrapper_setup.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facialrecognition_wrapper_training.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facialrecognition_wrapper_recognize.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facialrecognition_wrapper_identity.cpp

                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/kd_node.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/kd_tree.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/opencvdnnfacerecognizer.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/dnnfaceextractor.cpp

                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbaccess.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbbackend.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedboperationgroup.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbschemaupdater.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_identity.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_dnn.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_dnn_spatial.cpp
)

# Used by digikamgui
add_library(gui_digikamfacesenginedatabase_obj
            OBJECT
            ${facesengine_database_LIB_SRCS}
)

target_compile_definitions(gui_digikamfacesenginedatabase_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

# For unit-tests
add_library(digikamfacesenginedatabase STATIC $<TARGET_OBJECTS:gui_digikamfacesenginedatabase_obj>)
