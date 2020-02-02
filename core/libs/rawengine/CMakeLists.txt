#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

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

set(libraw_LIB_SRCS ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/libraw_c_api.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/libraw_datastream.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/canon_600.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/crx.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/decoders_dcraw.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/decoders_libraw_dcrdefs.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/decoders_libraw.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/dng.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/fp_dng.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/fuji_compressed.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/generic.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/kodak_decoders.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/load_mfbacks.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/smal.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/unpack_thumb.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/decoders/unpack.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/demosaic/aahd_demosaic.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/demosaic/ahd_demosaic.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/demosaic/dcb_demosaic.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/demosaic/dht_demosaic.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/demosaic/misc_demosaic.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/demosaic/xtrans_demosaic.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/integration/dngsdk_glue.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/integration/rawspeed_glue.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/adobepano.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/canon.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/ciff.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/cr3_parser.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/epson.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/exif_gps.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/fuji.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/identify_tools.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/identify.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/kodak.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/leica.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/makernotes.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/mediumformat.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/minolta.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/misc_parsers.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/nikon.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/normalize_model.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/olympus.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/hasselblad_model.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/p1.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/pentax.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/samsung.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/sony.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/metadata/tiff.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/postprocessing/aspect_ratio.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/postprocessing/dcraw_process.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/postprocessing/mem_image.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/postprocessing/postprocessing_aux.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/postprocessing/postprocessing_utils_dcrdefs.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/postprocessing/postprocessing_utils.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/preprocessing/ext_preprocess.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/preprocessing/raw2image.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/preprocessing/subtract_black.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/tables/cameralist.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/tables/colorconst.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/tables/colordata.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/tables/wblists.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/curves.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/decoder_info.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/init_close_utils.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/open.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/phaseone_processing.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/read_utils.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/thumb_utils.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/utils_dcraw.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/utils/utils_libraw.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/write/apply_profile.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/write/file_write.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/write/tiff_writer.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/x3f/x3f_parse_process.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/src/x3f/x3f_utils_patched.cpp
)

add_library(libraw_src OBJECT ${libraw_LIB_SRCS})

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
    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawdecoder.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawdecoder_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawdecodersettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawdecoderwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawinfo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawfiles.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(rawengine_srcs OBJECT ${librawengine_SRCS})
