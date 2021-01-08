#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

if(NOT MSVC)

    REMOVE_COMPILER_WARNINGS("-Wcast-align")
    REMOVE_COMPILER_WARNINGS("-Wzero-as-null-pointer-constant")

    DISABLE_CLANG_COMPILER_WARNINGS("4.99.99" "-Wno-keyword-macro")

endif()

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
)

set(libpgfutils_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/pgfutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libpgf/Decoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libpgf/Encoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libpgf/PGFimage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libpgf/PGFstream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libpgf/Subband.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libpgf/WaveletTransform.cpp
)

# Don't process automoc on Libpgf headers

file(GLOB_RECURSE libpgf_headers ${CMAKE_CURRENT_SOURCE_DIR}/libpgf/*.h)

foreach(_file ${libpgf_headers})
    set_property(SOURCE ${_file} PROPERTY SKIP_AUTOMOC ON)
endforeach()

# Used by digikamcore
add_library(core_pgfutils_obj OBJECT ${libpgfutils_SRCS})

target_compile_definitions(core_pgfutils_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
