#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# DNG SDK and XMP SDK use C++ exceptions
kde_enable_exceptions()

include_directories(
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

    ${EXPAT_INCLUDE_DIR}

    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/md5
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/common
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/include
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/include/client-glue
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/
)

#------------------------------------------------------------------------------------

set(libmd5_SRCS ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/md5/XMP_MD5.cpp)

add_library(libmd5_src OBJECT ${libmd5_SRCS})

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)
    target_compile_options(libmd5_src PRIVATE /w)
else()
    target_compile_options(libmd5_src PRIVATE -w -fPIC)
endif()

#------------------------------------------------------------------------------------

set(libxmp_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/common/XML_Node.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/common/UnicodeConversions.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/XMPCore_Impl.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/WXMPIterator.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/WXMPMeta.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/WXMPUtils.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/XMPIterator.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/XMPMeta-GetSet.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/XMPMeta-Parse.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/XMPMeta-Serialize.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/XMPMeta.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/XMPUtils-FileInfo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/XMPUtils.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/ExpatAdapter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/xmp_sdk/XMPCore/ParseRDF.cpp
)

add_library(libxmp_src OBJECT ${libxmp_SRCS})

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)
    target_compile_options(libxmp_src PRIVATE /w)
else()
    target_compile_options(libxmp_src PRIVATE -w -fPIC)
endif()

#------------------------------------------------------------------------------------

set(libdng_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_1d_function.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_date_time.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_ifd.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_memory.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_point.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_simple_image.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_utils.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_1d_table.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_exceptions.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_image.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_memory_stream.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_rational.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_spline.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_xmp.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_abort_sniffer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_exif.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_image_writer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_preview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_misc_opcodes.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_mosaic_info.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_read_image.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_stream.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_xmp_sdk.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_area_task.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_file_stream.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_info.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_mutex.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_rect.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_string.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_xy_coord.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_bottlenecks.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_bad_pixels.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_filter_task.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_iptc.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_negative.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_reference.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_string_list.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_camera_profile.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_fingerprint.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_lens_correction.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_linearization_info.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_opcode_list.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_opcodes.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_orientation.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_render.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_tag_types.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_color_space.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_globals.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_gain_map.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_lossless_jpeg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_parse_utils.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_resample.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_temperature.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_color_spec.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_host.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_matrix.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_pixel_buffer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_shared.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_tile_iterator.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_tone_curve.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_hue_sat_map.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_pthread.cpp
)

add_library(libdng_src OBJECT ${libdng_SRCS})

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)
    target_compile_options(libdng_src PRIVATE /w)
else()
    target_compile_options(libdng_src PRIVATE -w -fPIC)
endif()

# For unit tests.
add_library(libdng STATIC
            $<TARGET_OBJECTS:libdng_src>
            $<TARGET_OBJECTS:libxmp_src>
            $<TARGET_OBJECTS:libmd5_src>
)

#------------------------------------------------------------------------------------

set(libdngwriter_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/dngwriter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/dngwriter_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/dngwriter_convert.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/dngwriterhost.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/dngsettings.cpp
)

add_library(dngwriter_src OBJECT ${libdngwriter_SRCS})
