#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
)

set(raw2png_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/raw2png.cpp)
add_executable(raw2png ${raw2png_SRCS})
target_link_libraries(raw2png

                      digikamcore

                      ${COMMON_TEST_LINK}
)

set(libinfo_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libinfo.cpp)
add_executable(libinfo ${libinfo_SRCS})
target_link_libraries(libinfo

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -- LibRaw CLI Samples Compilation --------------------------------------------------------------------------------

# A small macro so that this is a bit cleaner

macro(LIBRAW_BUILD_SAMPLES)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)
    list(GET _temp 1 _ext)

    set(${_target}_SRCS ${CMAKE_SOURCE_DIR}/core/libs/rawengine/libraw/samples/${_filename})

    if (_ext STREQUAL "c")

        while(CMAKE_C_FLAGS MATCHES "-std=iso9899:1990")

            string(REPLACE "-std=iso9899:1990" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

        endwhile()

    endif()

    if(WIN32 AND MSVC)
        set_source_files_properties(${${_target}_SRCS} PROPERTIES COMPILE_FLAGS /w)
    else()
        set_source_files_properties(${${_target}_SRCS} PROPERTIES COMPILE_FLAGS "-w")
    endif()

    add_executable(${_target} ${${_target}_SRCS})

    target_link_libraries(${_target}

                          libraw

                          ${COMMON_TEST_LINK}
    )

endmacro()

LIBRAW_BUILD_SAMPLES(4channels.cpp)
LIBRAW_BUILD_SAMPLES(dcraw_emu.cpp)
LIBRAW_BUILD_SAMPLES(mem_image_sample.cpp)
LIBRAW_BUILD_SAMPLES(multirender_test.cpp)
LIBRAW_BUILD_SAMPLES(openbayer_sample.cpp)
LIBRAW_BUILD_SAMPLES(postprocessing_benchmark.cpp)
LIBRAW_BUILD_SAMPLES(raw-identify.cpp)
LIBRAW_BUILD_SAMPLES(rawtextdump.cpp)
LIBRAW_BUILD_SAMPLES(simple_dcraw.cpp)
LIBRAW_BUILD_SAMPLES(unprocessed_raw.cpp)

# C sample files which must be compiled only if LCMS v2 is detected, to prevent broken compilation

if(LCMS2_FOUND)

   if(MSVC)

        LIBRAW_BUILD_SAMPLES(half_mt_win32.c)

   else()

        LIBRAW_BUILD_SAMPLES(dcraw_half.c)
        LIBRAW_BUILD_SAMPLES(half_mt.c)

   endif()

endif()
