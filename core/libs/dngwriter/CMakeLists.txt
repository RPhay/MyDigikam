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

    ${CMAKE_CURRENT_SOURCE_DIR}/extra/md5
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/build
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/client-glue
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/Interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/sources
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/Interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/Interfaces/BaseInterfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/sources
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

#------------------------------------------------------------------------------------

set(libmd5_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/extra/md5/XMP_MD5.cpp)

# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)
foreach(_currentfile ${libmd5_SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

add_library(libmd5_src OBJECT ${libmd5_SRCS})

#------------------------------------------------------------------------------------

set(libxmp_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/Host_IO-POSIX.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/Host_IO-Win.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/IOUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/PerfUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/SafeStringAPIs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/UnicodeConversions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XML_Node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XMP_LibUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XMP_ProgressTracker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XMPFiles_IO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/ExpatAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/ParseRDF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/WXMPIterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/WXMPMeta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/WXMPUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPCore_Impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPIterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta-GetSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta-Parse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta-Serialize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPUtils-FileInfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPUtils.cpp
)

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)
foreach(_currentfile ${libxmp_SRCS})
    if(MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-w")
    else()
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-w -fPIC")
    endif()
endforeach()

add_library(libxmp_src OBJECT ${libxmp_SRCS})

#------------------------------------------------------------------------------------

set(libdng_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_1d_function.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_1d_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_abort_sniffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_area_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_bad_pixels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_big_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_bottlenecks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_camera_profile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_color_space.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_color_spec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_date_time.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_exceptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_exif.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_file_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_filter_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_fingerprint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_gain_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_globals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_host.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_hue_sat_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_ifd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_image_writer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_iptc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_jpeg_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_lens_correction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_linearization_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_local_string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_lossless_jpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_matrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_memory_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_misc_opcodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_mosaic_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_mutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_negative.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_opcode_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_opcodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_orientation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_parse_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_pixel_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_preview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_pthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_rational.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_read_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_rect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_ref_counted_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_reference.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_render.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_resample.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_safe_arithmetic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_shared.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_simple_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_spline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_string_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tag_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_temperature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tile_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tone_curve.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_validate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xmp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xmp_sdk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xy_coord.cpp
)

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)
foreach(_currentfile ${libdng_SRCS})
    if(MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-w")
    else()
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-w -fPIC")
    endif()
endforeach()

add_library(libdng_src OBJECT ${libdng_SRCS})

#------------------------------------------------------------------------------------

set(libdngwriter_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriterhost.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngsettings.cpp
)

add_library(dngwriter_src OBJECT ${libdngwriter_SRCS})
