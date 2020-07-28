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

    ${CMAKE_CURRENT_SOURCE_DIR}/extra/md5
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/common
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/client-glue
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
    target_compile_options(core_libmd5_obj PRIVATE -w)
endif()

#------------------------------------------------------------------------------------

if(MINGW)
    add_definitions("-D_POSIX_THREAD_SAFE_FUNCTIONS")
endif()

set(libxmp_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/common/XML_Node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/common/UnicodeConversions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPCore_Impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/WXMPIterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/WXMPMeta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/WXMPUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPIterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta-GetSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta-Parse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta-Serialize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPUtils-FileInfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/ExpatAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/ParseRDF.cpp
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
    target_compile_options(core_libxmp_obj PRIVATE -w)
endif()

#------------------------------------------------------------------------------------

set(libdng_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_1d_function.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_date_time.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_ifd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_simple_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_1d_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_exceptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_memory_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_rational.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_spline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xmp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_abort_sniffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_exif.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_image_writer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_preview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_misc_opcodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_mosaic_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_read_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xmp_sdk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_area_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_file_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_mutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_rect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xy_coord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_bottlenecks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_bad_pixels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_filter_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_iptc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_negative.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_reference.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_string_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_camera_profile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_fingerprint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_lens_correction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_linearization_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_opcode_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_opcodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_orientation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_render.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tag_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_color_space.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_globals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_gain_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_lossless_jpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_parse_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_resample.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_temperature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_color_spec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_host.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_matrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_pixel_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_shared.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tile_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tone_curve.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_hue_sat_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_pthread.cpp
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

    target_compile_options(core_libdng_obj PRIVATE -w)

endif()

# For unit tests.
add_library(libdng STATIC
            $<TARGET_OBJECTS:core_libdng_obj>
            $<TARGET_OBJECTS:core_libxmp_obj>
            $<TARGET_OBJECTS:core_libmd5_obj>
)

#------------------------------------------------------------------------------------

set(libdngwriter_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriterhost.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngsettings.cpp
)

# Used by digikamcore
add_library(core_dngwriter_obj OBJECT ${libdngwriter_SRCS})

target_compile_definitions(core_dngwriter_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
