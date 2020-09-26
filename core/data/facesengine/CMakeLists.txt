#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Shape predictor model data file

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/shape-predictor/shapepredictor.dat
        DESTINATION ${DATA_INSTALL_DIR}/digikam/facesengine
)

# Face recognition OpenFace model data file

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dnnface/openface_nn4.small2.v1.t7
        DESTINATION ${DATA_INSTALL_DIR}/digikam/facesengine
)

# Face detection SSD model data files

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dnnface/deploy.prototxt
        DESTINATION ${DATA_INSTALL_DIR}/digikam/facesengine
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dnnface/res10_300x300_ssd_iter_140000_fp16.caffemodel
        DESTINATION ${DATA_INSTALL_DIR}/digikam/facesengine
)

# Face detection Yolo model data files
# Disabled as SSD model are lightweight compared to Yolo model and provide very good results.

#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dnnface/yolov3-face.cfg
#        DESTINATION ${DATA_INSTALL_DIR}/digikam/facesengine
#)

#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dnnface/yolov3-wider_16000.weights
#        DESTINATION ${DATA_INSTALL_DIR}/digikam/facesengine
#)
