#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
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

set(facesengine_LIB_SRCS common/identity.cpp
                         common/dataproviders.cpp
                         common/opencvmatdata.cpp

                         detection/facedetector.cpp

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

if(DNN_DETECTION)

    add_definitions(-DUSE_DNN_DETECTION_BACKEND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/detection/opencv-dnn)

    set(facesengine_LIB_SRCS ${facesengine_LIB_SRCS}
                             detection/opencv-dnn/opencvdnnfacedetector.cpp
                             detection/opencv-dnn/dnnfacedetectorssd.cpp
                             detection/opencv-dnn/dnnfacedetectoryolo.cpp
                             detection/opencv-dnn/dnnfacedetectorbase.cpp
    )

else()

    include_directories(detection/opencv-face)

    set(facesengine_LIB_SRCS ${facesengine_LIB_SRCS}
                             detection/opencv-face/opencvfacedetector.cpp
                             detection/opencv-face/opencvfacedetector_p.cpp
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

set(facesengine_database_LIB_SRCS recognition/recognitiondatabase.cpp
                                  recognition/recognitiondatabase_p.cpp
                                  recognition/recognitiondatabase_identity.cpp
                                  recognition/recognitiondatabase_training.cpp
                                  recognition/recognitiondatabase_backend.cpp
                                  recognition/recognitiondatabase_recognize.cpp
                                  recognition/recognitiontrainingprovider.cpp


                                  alignment/congealing/funnelreal.cpp

                                  facedb/facedbaccess.cpp
                                  facedb/facedbbackend.cpp
                                  facedb/facedboperationgroup.cpp
                                  facedb/facedbschemaupdater.cpp
                                  facedb/facedb.cpp
                                  facedb/facedb_identity.cpp

                                  # Eigen Faces recognition module based on OpenCV (experimental and deprecated)

#                                  recognition/opencv-eigenfaces/eigenfacemodel.cpp
#                                  recognition/opencv-eigenfaces/opencveigenfacerecognizer.cpp
#                                  recognition/opencv-eigenfaces/facerec_eigenborrowed.cpp
#                                  facedb/facedb_eigen.cpp

                                  # Fisher Faces recognition module based on OpenCV (experimental and deprecated)

#                                  recognition/opencv-fisherfaces/fisherfacemodel.cpp
#                                  recognition/opencv-fisherfaces/opencvfisherfacerecognizer.cpp
#                                  recognition/opencv-fisherfaces/facerec_fisherborrowed.cpp
#                                  facedb/facedb_fisher.cpp

)

if(DNN_RECOGNITION)

    add_definitions(-DUSE_DNN_RECOGNITION_BACKEND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-dnn)

    set(facesengine_database_LIB_SRCS ${facesengine_database_LIB_SRCS}

                                      # Neural NetWork Faces recognition module based on OpenCV DNN

                                      facedb/facedb_dnn.cpp

                                      recognition/opencv-dnn/dnnfacemodel.cpp
                                      recognition/opencv-dnn/opencvdnnfacerecognizer.cpp
                                      recognition/opencv-dnn/facerec_dnnborrowed.cpp
                                      recognition/opencv-dnn/dnnfaceextractor.cpp
                                      recognition/opencv-dnn/dnndbscan.cpp
    )

else()

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-face
                        ${CMAKE_CURRENT_SOURCE_DIR}/recognition/opencv-lbph
    )

    set(facesengine_database_LIB_SRCS ${facesengine_database_LIB_SRCS}

                                      # Lpbh Faces recognition module based on OpenCV

                                      recognition/opencv-face/opencv_face.cpp
                                      facedb/facedb_lbph.cpp

                                      recognition/opencv-lbph/lbphfacemodel.cpp
                                      recognition/opencv-lbph/opencvlbphfacerecognizer.cpp
                                      recognition/opencv-lbph/facerec_borrowed.cpp
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
