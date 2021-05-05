#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# LibRaw use C++ exceptions
kde_enable_exceptions()

# -- General definitions rules ------------------------------------------------------------------------

# To prevent warnings from M$ compiler

if(WIN32 AND MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_ATL_SECURE_NO_WARNINGS)
    add_definitions(-D_AFX_SECURE_NO_WARNINGS)
endif()

# Under Windows, use specific flag to compile.

if(WIN32)
    add_definitions(-DDJGPP)
endif()

# Adjust compiler options to compile fine.

if(NOT MSVC)

    REMOVE_GCC_COMPILER_WARNINGS("-Wundef")
    REMOVE_GCC_COMPILER_WARNINGS("-Werror=return-type")
    DISABLE_CLANG_COMPILER_WARNINGS("3.99.99" "-Wno-c++11-narrowing")

endif()

# We always use LCMS version 2
add_definitions(-DUSE_LCMS2)

# Flag to compile with DNG SDK for special DNG files decoding. See README.DNGSDK.txt
#add_definitions(-DUSE_DNGSDK)

# Flag to debug LibRaw
add_definitions(-DDCRAW_VERBOSE)

# Flag used into LibRaw to be not thread-safe. Never use this mode.
#add_definitions(-DLIBRAW_NOTHREADS)

# Flags to enable extra codecs
add_definitions(-DUSE_X3FTOOLS)
add_definitions(-DUSE_6BY9RPI)

if(WIN32)
    # We need native UTF-16 encoding path support
    add_definitions(-DLIBRAW_WIN32_UNICODEPATHS)

    # No need a Flag to export library symbols, as we use a static version for the unit tests.
    add_definitions(-DLIBRAW_WIN32_DLLDEFS)
    add_definitions(-DLIBRAW_NODLL)
endif()

include_directories(${CMAKE_BINARY_DIR}/core/libs/rawengine/libraw/
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw
                    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk
)

set(libraw_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_c_api.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_datastream.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/canon_600.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/crx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/decoders_dcraw.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/decoders_libraw_dcrdefs.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/decoders_libraw.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/dng.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/fp_dng.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/fuji_compressed.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/generic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/kodak_decoders.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/load_mfbacks.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/smal.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/unpack_thumb.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/decoders/unpack.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/demosaic/aahd_demosaic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/demosaic/ahd_demosaic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/demosaic/dcb_demosaic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/demosaic/dht_demosaic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/demosaic/misc_demosaic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/demosaic/xtrans_demosaic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/integration/dngsdk_glue.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/integration/rawspeed_glue.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/adobepano.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/canon.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/ciff.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/cr3_parser.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/epson.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/exif_gps.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/fuji.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/identify_tools.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/identify.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/kodak.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/leica.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/makernotes.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/mediumformat.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/minolta.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/misc_parsers.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/nikon.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/normalize_model.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/olympus.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/hasselblad_model.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/p1.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/pentax.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/samsung.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/sony.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/metadata/tiff.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/postprocessing/aspect_ratio.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/postprocessing/dcraw_process.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/postprocessing/mem_image.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/postprocessing/postprocessing_aux.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/postprocessing/postprocessing_utils_dcrdefs.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/postprocessing/postprocessing_utils.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/preprocessing/ext_preprocess.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/preprocessing/raw2image.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/preprocessing/subtract_black.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/tables/cameralist.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/tables/colorconst.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/tables/colordata.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/tables/wblists.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/curves.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/decoder_info.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/init_close_utils.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/open.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/phaseone_processing.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/read_utils.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/thumb_utils.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/utils_dcraw.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/utils/utils_libraw.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/write/apply_profile.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/write/file_write.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/write/tiff_writer.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/x3f/x3f_parse_process.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/x3f/x3f_utils_patched.cpp
)

# Don't process automoc on Libraw headers

file(GLOB_RECURSE libraw_headers ${CMAKE_CURRENT_SOURCE_DIR}/libraw/*.h)

foreach(_file ${libraw_headers})
    set_property(SOURCE ${_file} PROPERTY SKIP_AUTOMOC ON)
endforeach()

# Used by digikamcore
add_library(core_libraw_obj OBJECT ${libraw_LIB_SRCS})

target_compile_definitions(core_libraw_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# Disable compilation warnings from LibRaw. Just to be clear on the console.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (Bug: #269903)
# Use O4 GCC compilation option to prevent artifacts with OpenMP (Bug #305382)
if(MSVC)
    target_compile_options(core_libraw_obj PRIVATE /w)
else()
    target_compile_options(core_libraw_obj PRIVATE -w -O4)
endif()

# For unit tests.
add_library(libraw STATIC
            $<TARGET_OBJECTS:core_libraw_obj>
)

#################################

set(librawengine_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/drawdecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drawdecoder_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drawdecodersettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drawdecoderwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drawinfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drawfiles.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(core_rawengine_obj OBJECT ${librawengine_SRCS})

target_compile_definitions(core_rawengine_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
