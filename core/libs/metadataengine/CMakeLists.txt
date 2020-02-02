#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

kde_enable_exceptions()

# TODO: How to extract exiv2lib definitions properties with cmake import interface?
#add_definitions($<TARGET_PROPERTY:exiv2lib,INTERFACE_COMPILE_DEFINITIONS>)

if(WIN32)
    add_definitions(-DEXV_HAVE_DLL)
endif()

# Exiv2 version < 0.28 use auto_ptr instead unique_ptr which is deprecated.

if("${exiv2_VERSION}" VERSION_LESS "0.27.99")

    DISABLE_GCC_COMPILER_WARNINGS("5.99.99" "-Wno-deprecated")

    # NOTE: under MacOS, this line has no effect... Why ?
    DISABLE_CLANG_COMPILER_WARNINGS("3.99.99" "-Wdeprecated-declarations")

endif()

set(libdmetadata_SRCS

    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_data.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_data_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_exif.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_fileio.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_item.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_comments.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_iptc.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_gps.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_xmp.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_previews.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_rotation.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaenginesettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaenginesettingscontainer.cpp
   
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_video.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_fileio.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_comments.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_template.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_exif.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_iptc.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_labels.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_faces.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_history.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_photo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_tags.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_generic.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadata_xmp.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadatasettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/dmetadatasettingscontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/dmetadata/geodetictools.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/containers/metadatainfo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/containers/photoinfocontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/containers/videoinfocontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/containers/template.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/containers/captionvalues.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:exiv2lib,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(dmetadata_src OBJECT ${libdmetadata_SRCS})
