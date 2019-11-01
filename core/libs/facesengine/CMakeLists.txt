#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

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

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/common
                    ${CMAKE_CURRENT_SOURCE_DIR}/facedb

                    ${CMAKE_CURRENT_SOURCE_DIR}/alignment/congealing

                    ${CMAKE_CURRENT_SOURCE_DIR}/detection
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-face

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

set(facesengine_LIB_SRCS common/identity.cpp
                         common/dataproviders.cpp

                         detection/facedetector.cpp
                         detection/opencvfacedetector.cpp
                         detection/opencvdnnfacedetector.cpp

                         detection/opencv-dnn/dnnfacedetectorssd.cpp
                         detection/opencv-dnn/dnnfacedetectoryolo.cpp
                         detection/opencv-dnn/dnnfacedetectorbase.cpp

                         detection/opencv-face/opencv_facerec.cpp
                         detection/opencv-face/opencv_predictcollector.cpp

                         preprocessing/recognition/recognitionpreprocessor.cpp
                         preprocessing/recognition/openfacepreprocessor.cpp

                         preprocessing/shape-predictor/matrixoperations.cpp
                         preprocessing/shape-predictor/pointtransformaffine.cpp
                         preprocessing/shape-predictor/qdatastreamoverloads.cpp
                         preprocessing/shape-predictor/fullobjectdetection.cpp
                         preprocessing/shape-predictor/shapepredictor.cpp

                         # alignment/flandmark/flandmarkaligner.cpp
                         # preprocessing/tantriggs/tantriggspreprocessor.cpp
)

set(facesengine_database_LIB_SRCS recognition/recognitiondatabase.cpp
                                  recognition/recognitiondatabase_p.cpp
                                  recognition/recognitiondatabase_identity.cpp
                                  recognition/recognitiondatabase_training.cpp
                                  recognition/recognitiondatabase_backend.cpp
                                  recognition/recognitiondatabase_recognize.cpp
                                  recognition/recognitiontrainingprovider.cpp

                                  # Eigen Faces recognition module based on OpenCV

                                  recognition/opencv-eigenfaces/eigenfacemodel.cpp
                                  recognition/opencv-eigenfaces/opencveigenfacerecognizer.cpp
                                  recognition/opencv-eigenfaces/facerec_eigenborrowed.cpp

                                  # Fisher Faces recognition module based on OpenCV

                                  recognition/opencv-fisherfaces/fisherfacemodel.cpp
                                  recognition/opencv-fisherfaces/opencvfisherfacerecognizer.cpp
                                  recognition/opencv-fisherfaces/facerec_fisherborrowed.cpp


                                  # Lpbh Faces recognition module based on OpenCV

                                  recognition/opencv-lbph/lbphfacemodel.cpp
                                  recognition/opencv-lbph/opencvlbphfacerecognizer.cpp
                                  recognition/opencv-lbph/opencvmatdata.cpp
                                  recognition/opencv-lbph/facerec_borrowed.cpp

                                  # Neural NetWork Faces recognition module based on OpenCV DNN

                                  recognition/opencv-dnn/dnnfacemodel.cpp
                                  recognition/opencv-dnn/opencvdnnfacerecognizer.cpp
                                  recognition/opencv-dnn/facerec_dnnborrowed.cpp
                                  recognition/opencv-dnn/dnnfaceextractor.cpp
                                  recognition/opencv-dnn/dnndbscan.cpp

                                  alignment/congealing/funnelreal.cpp

                                  facedb/facedbaccess.cpp
                                  facedb/facedbbackend.cpp
                                  facedb/facedboperationgroup.cpp
                                  facedb/facedbschemaupdater.cpp
                                  facedb/facedb.cpp
                                  facedb/facedb_identity.cpp
                                  facedb/facedb_lbph.cpp
                                  facedb/facedb_eigen.cpp
                                  facedb/facedb_fisher.cpp
                                  facedb/facedb_dnn.cpp
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
