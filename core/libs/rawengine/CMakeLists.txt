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

# No need a Flag to export library symbols, as we use a static version for the unit tests.
add_definitions(-DLIBRAW_NODLL)

include_directories(${CMAKE_BINARY_DIR}/core/libs/rawengine/libraw/
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw
                    ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk
)

set(libraw_LIB_SRCS libraw/src/libraw_c_api.cpp
                    libraw/src/libraw_datastream.cpp
                    libraw/src/decoders/canon_600.cpp
                    libraw/src/decoders/crx.cpp
                    libraw/src/decoders/decoders_dcraw.cpp
                    libraw/src/decoders/decoders_libraw_dcrdefs.cpp
                    libraw/src/decoders/decoders_libraw.cpp
                    libraw/src/decoders/dng.cpp
                    libraw/src/decoders/fp_dng.cpp
                    libraw/src/decoders/fuji_compressed.cpp
                    libraw/src/decoders/generic.cpp
                    libraw/src/decoders/kodak_decoders.cpp
                    libraw/src/decoders/load_mfbacks.cpp
                    libraw/src/decoders/smal.cpp
                    libraw/src/decoders/unpack_thumb.cpp
                    libraw/src/decoders/unpack.cpp
                    libraw/src/demosaic/aahd_demosaic.cpp
                    libraw/src/demosaic/ahd_demosaic.cpp
                    libraw/src/demosaic/dcb_demosaic.cpp
                    libraw/src/demosaic/dht_demosaic.cpp
                    libraw/src/demosaic/misc_demosaic.cpp
                    libraw/src/demosaic/xtrans_demosaic.cpp
                    libraw/src/integration/dngsdk_glue.cpp
                    libraw/src/integration/rawspeed_glue.cpp
                    libraw/src/metadata/adobepano.cpp
                    libraw/src/metadata/canon.cpp
                    libraw/src/metadata/ciff.cpp
                    libraw/src/metadata/cr3_parser.cpp
                    libraw/src/metadata/epson.cpp
                    libraw/src/metadata/exif_gps.cpp
                    libraw/src/metadata/fuji.cpp
                    libraw/src/metadata/identify_tools.cpp
                    libraw/src/metadata/identify.cpp
                    libraw/src/metadata/kodak.cpp
                    libraw/src/metadata/leica.cpp
                    libraw/src/metadata/makernotes.cpp
                    libraw/src/metadata/mediumformat.cpp
                    libraw/src/metadata/minolta.cpp
                    libraw/src/metadata/misc_parsers.cpp
                    libraw/src/metadata/nikon.cpp
                    libraw/src/metadata/normalize_model.cpp
                    libraw/src/metadata/olympus.cpp
                    libraw/src/metadata/hasselblad_model.cpp
                    libraw/src/metadata/p1.cpp
                    libraw/src/metadata/pentax.cpp
                    libraw/src/metadata/samsung.cpp
                    libraw/src/metadata/sony.cpp
                    libraw/src/metadata/tiff.cpp
                    libraw/src/postprocessing/aspect_ratio.cpp
                    libraw/src/postprocessing/dcraw_process.cpp
                    libraw/src/postprocessing/mem_image.cpp
                    libraw/src/postprocessing/postprocessing_aux.cpp
                    libraw/src/postprocessing/postprocessing_utils_dcrdefs.cpp
                    libraw/src/postprocessing/postprocessing_utils.cpp
                    libraw/src/preprocessing/ext_preprocess.cpp
                    libraw/src/preprocessing/raw2image.cpp
                    libraw/src/preprocessing/subtract_black.cpp
                    libraw/src/tables/cameralist.cpp
                    libraw/src/tables/colorconst.cpp
                    libraw/src/tables/colordata.cpp
                    libraw/src/tables/wblists.cpp
                    libraw/src/utils/curves.cpp
                    libraw/src/utils/decoder_info.cpp
                    libraw/src/utils/init_close_utils.cpp
                    libraw/src/utils/open.cpp
                    libraw/src/utils/phaseone_processing.cpp
                    libraw/src/utils/read_utils.cpp
                    libraw/src/utils/thumb_utils.cpp
                    libraw/src/utils/utils_dcraw.cpp
                    libraw/src/utils/utils_libraw.cpp
                    libraw/src/write/apply_profile.cpp
                    libraw/src/write/file_write.cpp
                    libraw/src/write/tiff_writer.cpp
                    libraw/src/x3f/x3f_parse_process.cpp
                    libraw/src/x3f/x3f_utils_patched.cpp
)

# Dont process automoc on Libraw headers

file(GLOB_RECURSE libraw_headers ${CMAKE_CURRENT_SOURCE_DIR}/libraw/*.h)

foreach(_file ${libraw_headers})
    set_property(SOURCE ${_file} PROPERTY SKIP_AUTOMOC ON)
endforeach()

# Used by digikamcore
add_library(libraw_src OBJECT ${libraw_LIB_SRCS})

target_compile_definitions(libraw_src
                           PRIVATE
                           digikamcore_EXPORTS
)

# Disable compilation warnings from LibRaw. Just to be clear on the console.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (Bug: #269903)
# Use O4 GCC compilation option to prevent artifacts with OpenMP (Bug #305382)
if(MSVC)
    target_compile_options(libraw_src PRIVATE /w)
else()
    target_compile_options(libraw_src PRIVATE -w -fPIC -O4)
endif()

# For unit tests.
add_library(libraw STATIC
            $<TARGET_OBJECTS:libraw_src>
)

#################################

set(librawengine_SRCS
    drawdecoder.cpp
    drawdecoder_p.cpp
    drawdecodersettings.cpp
    drawdecoderwidget.cpp
    drawinfo.cpp
    drawfiles.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(rawengine_srcs OBJECT ${librawengine_SRCS})

target_compile_definitions(rawengine_srcs
                           PRIVATE
                           digikamcore_EXPORTS
)
