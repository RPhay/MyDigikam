#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


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

include_directories(${CMAKE_SOURCE_DIR}/core/libs/facesengine/..
                    ${CMAKE_SOURCE_DIR}/core/libs/facesengine/common
                    ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb

                    ${CMAKE_SOURCE_DIR}/core/libs/facesengine/alignment/congealing

                    ${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection

                    ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing
                    ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/shape-predictor
                    ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/recognition

                    # ${CMAKE_SOURCE_DIR}/core/libs/facesengine/alignment/flandmark
                    # ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/tantriggs
)

set(facesengine_LIB_SRCS ${CMAKE_SOURCE_DIR}/core/libs/facesengine/common/identity.cpp
                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/common/dataproviders.cpp
                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/common/opencvmatdata.cpp

                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/facedetector.cpp

                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/recognition/recognitionpreprocessor.cpp
                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/recognition/openfacepreprocessor.cpp

                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/shape-predictor/matrixoperations.cpp
                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/shape-predictor/pointtransformaffine.cpp
                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/shape-predictor/qdatastreamoverloads.cpp
                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/shape-predictor/fullobjectdetection.cpp
                         ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/shape-predictor/shapepredictor.cpp

                         # ${CMAKE_SOURCE_DIR}/core/libs/facesengine/alignment/flandmark/flandmarkaligner.cpp
                         # ${CMAKE_SOURCE_DIR}/core/libs/facesengine/preprocessing/tantriggs/tantriggspreprocessor.cpp
)

if(DNN_DETECTION)

    add_definitions(-DUSE_DNN_DETECTION_BACKEND)
    include_directories(${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/opencv-dnn)

    set(facesengine_LIB_SRCS ${facesengine_LIB_SRCS}
                             ${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/opencv-dnn/opencvdnnfacedetector.cpp
                             ${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/opencv-dnn/dnnfacedetectorssd.cpp
                             ${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/opencv-dnn/dnnfacedetectoryolo.cpp
                             ${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/opencv-dnn/dnnfacedetectorbase.cpp
    )

else()

    include_directories(${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/opencv-face)

    set(facesengine_LIB_SRCS ${facesengine_LIB_SRCS}
                             ${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/opencv-face/opencvfacedetector.cpp
                             ${CMAKE_SOURCE_DIR}/core/libs/facesengine/detection/opencv-face/opencvfacedetector_p.cpp
    )

endif()

set(facesengine_database_LIB_SRCS ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/recognitiondatabase.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/recognitiondatabase_p.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/recognitiondatabase_identity.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/recognitiondatabase_training.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/recognitiondatabase_backend.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/recognitiondatabase_recognize.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/recognitiontrainingprovider.cpp


                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/alignment/congealing/funnelreal.cpp

                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedbaccess.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedbbackend.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedboperationgroup.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedbschemaupdater.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedb.cpp
                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedb_identity.cpp

                                  # Eigen Faces recognition module based on OpenCV (experimental and deprecated)

#                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-eigenfaces/eigenfacemodel.cpp
#                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-eigenfaces/opencveigenfacerecognizer.cpp
#                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-eigenfaces/facerec_eigenborrowed.cpp
#                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedb_eigen.cpp

                                  # Fisher Faces recognition module based on OpenCV (experimental and deprecated)

#                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-fisherfaces/fisherfacemodel.cpp
#                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-fisherfaces/opencvfisherfacerecognizer.cpp
#                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-fisherfaces/facerec_fisherborrowed.cpp
#                                  ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedb_fisher.cpp

)

if(DNN_RECOGNITION)

    add_definitions(-DUSE_DNN_RECOGNITION_BACKEND)
    include_directories(${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-dnn)

    set(facesengine_database_LIB_SRCS ${facesengine_database_LIB_SRCS}

                                      # Neural NetWork Faces recognition module based on OpenCV DNN

                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedb_dnn.cpp

                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-dnn/dnnfacemodel.cpp
                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-dnn/opencvdnnfacerecognizer.cpp
                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-dnn/facerec_dnnborrowed.cpp
                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-dnn/dnnfaceextractor.cpp
                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-dnn/dnndbscan.cpp
    )

else()

    include_directories(${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-face
                        ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-lbph
    )

    set(facesengine_database_LIB_SRCS ${facesengine_database_LIB_SRCS}

                                      # Lpbh Faces recognition module based on OpenCV

                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-face/opencv_face.cpp
                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/facedb/facedb_lbph.cpp

                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-lbph/lbphfacemodel.cpp
                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-lbph/opencvlbphfacerecognizer.cpp
                                      ${CMAKE_SOURCE_DIR}/core/libs/facesengine/recognition/opencv-lbph/facerec_borrowed.cpp
    )

endif()


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
