#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

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

    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_data_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_exif.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_fileio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_item.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_comments.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_iptc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_gps.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_xmp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_previews.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaengine_rotation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaenginesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/metaenginesettingscontainer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_video.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_libheif.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_libraw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_fileio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_comments.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_template.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_exif.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_iptc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_labels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_faces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_history.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_photo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_tags.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_generic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadata_xmp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadatasettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/dmetadatasettingscontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dmetadata/geodetictools.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/containers/metadatainfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/containers/photoinfocontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/containers/videoinfocontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/containers/template.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/containers/captionvalues.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:exiv2lib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(core_dmetadata_obj OBJECT ${libdmetadata_SRCS})

add_dependencies(core_dmetadata_obj core_libheif_obj)

target_compile_definitions(core_dmetadata_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
