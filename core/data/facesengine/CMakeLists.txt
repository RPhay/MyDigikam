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

# TODO: finalise this routine
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dnnface/yolov3-face.cfg
        DESTINATION ${DATA_INSTALL_DIR}/digikam/facesengine
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dnnface/yolov3-wider_16000.weights
        DESTINATION ${DATA_INSTALL_DIR}/digikam/facesengine
)

# ---------------------------------------------------------------

# NOTE: files not used for the moment.

# add_custom_command(OUTPUT face-funnel.data
#                    COMMAND ${CMAKE_COMMAND} -E tar xz ${CMAKE_CURRENT_SOURCE_DIR}/alignment/face-funnel.data.tgz)
#
# add_custom_target(extract_funnel_data ALL DEPENDS face-funnel.data)
#
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/face-funnel.data
#         DESTINATION ${DATA_INSTALL_DIR}/digikam/alignment-congealing
# )
#
# add_custom_command(OUTPUT flandmark_model.dat
#                    COMMAND ${CMAKE_COMMAND} -E tar xz ${CMAKE_CURRENT_SOURCE_DIR}/alignment/flandmark_model.dat.tgz)
#
# add_custom_target(extract_flandmark_data ALL DEPENDS flandmark_model.dat)
#
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/flandmark_model.dat
#         DESTINATION ${DATA_INSTALL_DIR}/digikam/alignment-flandmark)
