#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# Option to switch between DNN or HAAR face detection.
# DNN is the default one, Haar is the legacy deprecated method.
set(DNN_DETECTION TRUE)

# Option to switch between DNN or LBPH face recognition.
# DNN is the default one, LBPH is the legacy deprecated method.
set(DNN_RECOGNITION TRUE)

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
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/common
                    ${CMAKE_CURRENT_SOURCE_DIR}/facedb

                    ${CMAKE_CURRENT_SOURCE_DIR}/alignment/congealing

                    ${CMAKE_CURRENT_SOURCE_DIR}/detection

                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing
                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor
                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/recognition

                    # ${CMAKE_CURRENT_SOURCE_DIR}/alignment/flandmark
                    # ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/tantriggs
)

# ---------------------------------------------------------------------------

set(facesengine_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/common/identity.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/common/dataproviders.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/common/opencvmatdata.cpp

                         ${CMAKE_CURRENT_SOURCE_DIR}/detection/facedetector.cpp

                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/recognition/recognitionpreprocessor.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/recognition/openfacepreprocessor.cpp

                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/matrixoperations.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/pointtransformaffine.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/qdatastreamoverloads.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/fullobjectdetection.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor/shapepredictor.cpp

                         # ${CMAKE_CURRENT_SOURCE_DIR}/alignment/flandmark/flandmarkaligner.cpp
                         # ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/tantriggs/tantriggspreprocessor.cpp
)

if(DNN_DETECTION)

    add_definitions(-DUSE_DNN_DETECTION_BACKEND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn)

    set(facesengine_LIB_SRCS ${facesengine_LIB_SRCS}
                             ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn/opencvdnnfacedetector.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn/dnnfacedetectorssd.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn/dnnfacedetectoryolo.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn/dnnfacedetectorbase.cpp
    )

else()

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-face)

    set(facesengine_LIB_SRCS ${facesengine_LIB_SRCS}
                             ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-face/opencvfacedetector.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-face/opencvfacedetector_p.cpp
    )

endif()

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

set(facesengine_database_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_p.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_identity.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_training.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_backend.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_recognize.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiontrainingprovider.cpp

                                  #Replacement of RecognitionDatabase
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facesengine_interface.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facesengine_interface_p.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facesengine_interface_setup.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facesengine_interface_training.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facesengine_interface_recognize.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/facesengine_interface_identity.cpp

                                  ${CMAKE_CURRENT_SOURCE_DIR}/alignment/congealing/funnelreal.cpp

                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbaccess.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbbackend.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedboperationgroup.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbschemaupdater.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_identity.cpp

                                  # Eigen Faces recognition module based on OpenCV (experimental and deprecated)

#                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-eigenfaces/eigenfacemodel.cpp
#                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-eigenfaces/opencveigenfacerecognizer.cpp
#                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-eigenfaces/facerec_eigenborrowed.cpp
#                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_eigen.cpp

                                  # Fisher Faces recognition module based on OpenCV (experimental and deprecated)

#                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-fisherfaces/fisherfacemodel.cpp
#                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-fisherfaces/opencvfisherfacerecognizer.cpp
#                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-fisherfaces/facerec_fisherborrowed.cpp
#                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_fisher.cpp

)

if(DNN_RECOGNITION)

    add_definitions(-DUSE_DNN_RECOGNITION_BACKEND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn
                        # For testing TODO: remove later
                        ${CMAKE_CURRENT_SOURCE_DIR}/facerecognition)

    set(facesengine_database_LIB_SRCS ${facesengine_database_LIB_SRCS}

                                      # Neural NetWork Faces recognition module based on OpenCV DNN

                                      ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_dnn.cpp

                                      #${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/dnnfacemodel.cpp
                                      #${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/opencvdnnfacerecognizer.cpp
                                      #${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/facerec_dnnborrowed.cpp
                                      #${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/dnnfaceextractor.cpp
                                      #${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/dnndbscan.cpp

                                      # Testing, TODO: remove later
                                      ${CMAKE_CURRENT_SOURCE_DIR}/facerecognition/kd_node.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/facerecognition/kd_tree.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/facerecognition/facerecognizer.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/facerecognition/faceextractor.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/facerecognition/facedatabase.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/facerecognition/faceembedding_db.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/facerecognition/spatial_database.cpp
    )

else()

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-face
                        ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph
    )

    set(facesengine_database_LIB_SRCS ${facesengine_database_LIB_SRCS}

                                      # Lpbh Faces recognition module based on OpenCV

                                      ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-face/opencv_face.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_lbph.cpp

                                      ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph/lbphfacemodel.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph/opencvlbphfacerecognizer.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph/facerec_borrowed.cpp
    )

endif()


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
