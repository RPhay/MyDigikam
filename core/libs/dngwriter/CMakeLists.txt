#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# DNG SDK and XMP SDK use C++ exceptions
kde_enable_exceptions()

include_directories(
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

    ${EXPAT_INCLUDE_DIR}

    extra/md5
    extra/dng_sdk
    extra/xmp_sdk/XMPCore
    extra/xmp_sdk/common
    extra/xmp_sdk/include
    extra/xmp_sdk/include/client-glue
)

#------------------------------------------------------------------------------------

set(libmd5_SRCS extra/md5/XMP_MD5.cpp)

# Used by digikamcore
add_library(core_libmd5_obj OBJECT ${libmd5_SRCS})

target_compile_definitions(core_libmd5_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)
    target_compile_options(core_libmd5_obj PRIVATE /w)
else()
    target_compile_options(core_libmd5_obj PRIVATE -w -fPIC)
endif()

#------------------------------------------------------------------------------------

set(libxmp_SRCS
    extra/xmp_sdk/common/XML_Node.cpp
    extra/xmp_sdk/common/UnicodeConversions.cpp
    extra/xmp_sdk/XMPCore/XMPCore_Impl.cpp
    extra/xmp_sdk/XMPCore/WXMPIterator.cpp
    extra/xmp_sdk/XMPCore/WXMPMeta.cpp
    extra/xmp_sdk/XMPCore/WXMPUtils.cpp
    extra/xmp_sdk/XMPCore/XMPIterator.cpp
    extra/xmp_sdk/XMPCore/XMPMeta-GetSet.cpp
    extra/xmp_sdk/XMPCore/XMPMeta-Parse.cpp
    extra/xmp_sdk/XMPCore/XMPMeta-Serialize.cpp
    extra/xmp_sdk/XMPCore/XMPMeta.cpp
    extra/xmp_sdk/XMPCore/XMPUtils-FileInfo.cpp
    extra/xmp_sdk/XMPCore/XMPUtils.cpp
    extra/xmp_sdk/XMPCore/ExpatAdapter.cpp
    extra/xmp_sdk/XMPCore/ParseRDF.cpp
)

# Used by digikamcore
add_library(core_libxmp_obj OBJECT ${libxmp_SRCS})

target_compile_definitions(core_libxmp_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)
    target_compile_options(core_libxmp_obj PRIVATE /w)
else()
    target_compile_options(core_libxmp_obj PRIVATE -w -fPIC)
endif()

#------------------------------------------------------------------------------------

set(libdng_SRCS
    extra/dng_sdk/dng_1d_function.cpp
    extra/dng_sdk/dng_date_time.cpp
    extra/dng_sdk/dng_ifd.cpp
    extra/dng_sdk/dng_memory.cpp
    extra/dng_sdk/dng_point.cpp
    extra/dng_sdk/dng_simple_image.cpp
    extra/dng_sdk/dng_utils.cpp
    extra/dng_sdk/dng_1d_table.cpp
    extra/dng_sdk/dng_exceptions.cpp
    extra/dng_sdk/dng_image.cpp
    extra/dng_sdk/dng_memory_stream.cpp
    extra/dng_sdk/dng_rational.cpp
    extra/dng_sdk/dng_spline.cpp
    extra/dng_sdk/dng_xmp.cpp
    extra/dng_sdk/dng_abort_sniffer.cpp
    extra/dng_sdk/dng_exif.cpp
    extra/dng_sdk/dng_image_writer.cpp
    extra/dng_sdk/dng_preview.cpp
    extra/dng_sdk/dng_misc_opcodes.cpp
    extra/dng_sdk/dng_mosaic_info.cpp
    extra/dng_sdk/dng_read_image.cpp
    extra/dng_sdk/dng_stream.cpp
    extra/dng_sdk/dng_xmp_sdk.cpp
    extra/dng_sdk/dng_area_task.cpp
    extra/dng_sdk/dng_file_stream.cpp
    extra/dng_sdk/dng_info.cpp
    extra/dng_sdk/dng_mutex.cpp
    extra/dng_sdk/dng_rect.cpp
    extra/dng_sdk/dng_string.cpp
    extra/dng_sdk/dng_xy_coord.cpp
    extra/dng_sdk/dng_bottlenecks.cpp
    extra/dng_sdk/dng_bad_pixels.cpp
    extra/dng_sdk/dng_filter_task.cpp
    extra/dng_sdk/dng_iptc.cpp
    extra/dng_sdk/dng_negative.cpp
    extra/dng_sdk/dng_reference.cpp
    extra/dng_sdk/dng_string_list.cpp
    extra/dng_sdk/dng_camera_profile.cpp
    extra/dng_sdk/dng_fingerprint.cpp
    extra/dng_sdk/dng_lens_correction.cpp
    extra/dng_sdk/dng_linearization_info.cpp
    extra/dng_sdk/dng_opcode_list.cpp
    extra/dng_sdk/dng_opcodes.cpp
    extra/dng_sdk/dng_orientation.cpp
    extra/dng_sdk/dng_render.cpp
    extra/dng_sdk/dng_tag_types.cpp
    extra/dng_sdk/dng_color_space.cpp
    extra/dng_sdk/dng_globals.cpp
    extra/dng_sdk/dng_gain_map.cpp
    extra/dng_sdk/dng_lossless_jpeg.cpp
    extra/dng_sdk/dng_parse_utils.cpp
    extra/dng_sdk/dng_resample.cpp
    extra/dng_sdk/dng_temperature.cpp
    extra/dng_sdk/dng_color_spec.cpp
    extra/dng_sdk/dng_host.cpp
    extra/dng_sdk/dng_matrix.cpp
    extra/dng_sdk/dng_pixel_buffer.cpp
    extra/dng_sdk/dng_shared.cpp
    extra/dng_sdk/dng_tile_iterator.cpp
    extra/dng_sdk/dng_tone_curve.cpp
    extra/dng_sdk/dng_hue_sat_map.cpp
    extra/dng_sdk/dng_pthread.cpp
)

# Don't process automoc on headers from extra subdir

file(GLOB_RECURSE extra_headers ${CMAKE_CURRENT_SOURCE_DIR}/extra/*.h)

foreach(_file ${extra_headers})
    set_property(SOURCE ${_file} PROPERTY SKIP_AUTOMOC ON)
endforeach()

# Used by digikamcore
add_library(core_libdng_obj OBJECT ${libdng_SRCS})

target_compile_definitions(core_libdng_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)

    target_compile_options(core_libdng_obj PRIVATE /w)

else()

    target_compile_options(core_libdng_obj PRIVATE -w -fPIC)

endif()

# For unit tests.
add_library(libdng STATIC
            $<TARGET_OBJECTS:core_libdng_obj>
            $<TARGET_OBJECTS:core_libxmp_obj>
            $<TARGET_OBJECTS:core_libmd5_obj>
)

#------------------------------------------------------------------------------------

set(libdngwriter_SRCS
    dngwriter.cpp
    dngwriter_p.cpp
    dngwriter_convert.cpp
    dngwriterhost.cpp
    dngsettings.cpp
)

# Used by digikamcore
add_library(core_dngwriter_obj OBJECT ${libdngwriter_SRCS})

target_compile_definitions(core_dngwriter_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
