#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

kde_enable_exceptions()

# option to switch betweeen DNN or HAAR face detection.
# DNN is the default one, Haar is the legacy deprecated method.
set(DNN_DETECTION TRUE)

include_directories($<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/common
                    ${CMAKE_CURRENT_SOURCE_DIR}/facedb

                    ${CMAKE_CURRENT_SOURCE_DIR}/alignment/congealing

                    ${CMAKE_CURRENT_SOURCE_DIR}/detection

                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-face
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-eigenfaces
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-fisherfaces
                    ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn

                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing
                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/shape-predictor
                    ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/recognition

                    # ${CMAKE_CURRENT_SOURCE_DIR}/alignment/flandmark
                    # ${CMAKE_CURRENT_SOURCE_DIR}/preprocessing/tantriggs
)

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


set(facesengine_database_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_p.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_identity.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_training.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_backend.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiondatabase_recognize.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/recognitiontrainingprovider.cpp

                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-face/opencv_face.cpp

                                  # Eigen Faces recognition module based on OpenCV

                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-eigenfaces/eigenfacemodel.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-eigenfaces/opencveigenfacerecognizer.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-eigenfaces/facerec_eigenborrowed.cpp

                                  # Fisher Faces recognition module based on OpenCV

                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-fisherfaces/fisherfacemodel.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-fisherfaces/opencvfisherfacerecognizer.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-fisherfaces/facerec_fisherborrowed.cpp

                                  # Lpbh Faces recognition module based on OpenCV

                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph/lbphfacemodel.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph/opencvlbphfacerecognizer.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph/facerec_borrowed.cpp

                                  # Neural NetWork Faces recognition module based on OpenCV DNN

                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/dnnfacemodel.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/opencvdnnfacerecognizer.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/facerec_dnnborrowed.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/dnnfaceextractor.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn/dnndbscan.cpp

                                  ${CMAKE_CURRENT_SOURCE_DIR}/alignment/congealing/funnelreal.cpp

                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbaccess.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbbackend.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedboperationgroup.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedbschemaupdater.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_identity.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_lbph.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_eigen.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_fisher.cpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/facedb/facedb_dnn.cpp
)

add_library(digikamfacesengine_src
            OBJECT
            ${facesengine_LIB_SRCS}
)

add_library(digikamfacesengine STATIC $<TARGET_OBJECTS:digikamfacesengine_src>)

add_library(digikamfacesenginedatabase_src
            OBJECT
            ${facesengine_database_LIB_SRCS}
)

add_library(digikamfacesenginedatabase STATIC $<TARGET_OBJECTS:digikamfacesenginedatabase_src>)
