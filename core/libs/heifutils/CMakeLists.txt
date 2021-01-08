#
# Copyright (c) 2015-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(CheckFunctionExists)
include(CheckIncludeFile)

# Common libheif and libde265 system features detection #############################

CHECK_FUNCTION_EXISTS(posix_memalign HAVE_POSIX_MEMALIGN)
CHECK_INCLUDE_FILE(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILE(stdbool.h HAVE_STDBOOL_H)
CHECK_INCLUDE_FILE(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(stddef.h HAVE_STDDEF_H)
CHECK_INCLUDE_FILE(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)

if(HAVE_INTTYPES_H)
    add_definitions(-DHAVE_INTTYPES_H)
endif()

if(HAVE_STDDEF_H)
    add_definitions(-DHAVE_STDDEF_H)
endif()

if(HAVE_STRINGS_H)
    add_definitions(-DHAVE_STRINGS_H)
endif()

if(HAVE_UNISTD_H)
    add_definitions(-DHAVE_UNISTD_H)
endif()

if(HAVE_MALLOC_H)
    add_definitions(-DHAVE_MALLOC_H)
endif()

if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

if(HAVE_STDBOOL_H)
    add_definitions(-DHAVE_STDBOOL_H)
endif()

if(HAVE_POSIX_MEMALIGN)
    add_definitions(-DHAVE_POSIX_MEMALIGN)
endif()

# Libheif compilation rules ##########################################################

set(NUMERIC_VERSION 0x01060200)
set(PACKAGE_VERSION 1.6.2)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/libheif/heif_version.h"
)

add_definitions(-DLIBHEIF_EXPORTS -DHAVE_VISIBILITY)

set(libheif_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/bitstream.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/box.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/error.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_context.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_image.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_hevc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_colorconversion.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_plugin_registry.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_plugin.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_decoder_libde265.cc
)

if(X265_FOUND)
    include_directories(${X265_INCLUDE_DIRS})
    set(libheif_SRCS ${libheif_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/libheif/heif_encoder_x265.cc
    )
endif()

# Don't process automoc on libheif headers

file(GLOB_RECURSE heif_headers ${CMAKE_CURRENT_SOURCE_DIR}/libheif/*.h)

foreach(_file ${heif_headers})
    set_property(SOURCE ${_file} PROPERTY SKIP_AUTOMOC ON)
endforeach()

add_library(core_libheif_obj OBJECT ${libheif_SRCS})

if(MSVC)

    target_compile_options(core_libheif_obj PRIVATE /w)

else()

    target_compile_options(core_libheif_obj PRIVATE -w)

endif()

target_compile_definitions(core_libheif_obj
                           PRIVATE
                           HAVE_LIBDE265=1
)

if(X265_FOUND)

    target_compile_definitions(core_libheif_obj
                               PRIVATE
                               HAVE_X265=1
    )

endif()

# Used by digikamcore
target_compile_definitions(core_libheif_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# Libde265 compilation rules ##########################################################

set(NUMERIC_VERSION 0x01000500)
set(PACKAGE_VERSION 1.0.5)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libde265/de265-version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/libde265/de265-version.h"
)

add_definitions(-DLIBDE265_STATIC_BUILD)

set(libde265_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/bitstream.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/cabac.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/de265.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/deblock.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/decctx.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/nal-parser.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/dpb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/image.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/intrapred.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/md5.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/nal.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/pps.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/transform.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/refpic.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/sao.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/scan.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/sei.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/slice.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/sps.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/util.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/vps.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/vui.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/motion.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/threads.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/visualize.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/fallback.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/fallback-motion.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/fallback-dct.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/quality.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/configparam.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/image-io.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/alloc_pool.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/en265.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/contextmodel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/encoder-core.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/encoder-types.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/encoder-params.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/encoder-context.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/encoder-syntax.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/encoder-intrapred.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/encoder-motion.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/encpicbuf.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/sop.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/algo.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/coding-options.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/ctb-qscale.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/cb-split.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/cb-intrapartmode.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/cb-interpartmode.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/cb-skip.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/cb-intra-inter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/cb-mergeindex.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/tb-split.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/tb-transform.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/tb-intrapredmode.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/tb-rateestim.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/libde265/encoder/algo/pb-mv.cc
)

if(WIN32)
    set(libde265_SRCS ${libde265_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/libde265/extra/win32cond.cc
    )
endif()

# Don't process automoc on libde265 headers

file(GLOB_RECURSE de265_headers ${CMAKE_CURRENT_SOURCE_DIR}/libde265/*.h)

foreach(_file ${de265_headers})
    set_property(SOURCE ${_file} PROPERTY SKIP_AUTOMOC ON)
endforeach()

add_library(core_libde265_obj OBJECT ${libde265_SRCS})

if(MSVC)

    target_compile_options(core_libde265_obj PRIVATE /w /wd4267 /wd4244)

else()

    target_compile_options(core_libde265_obj PRIVATE -w)

endif()

# Used by digikamcore
target_compile_definitions(core_libde265_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

