#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# Define libjpeg_EXPORTS: kde-win emerged libjpeg uses this define to
# decide whether to make dllimport (by default) or dllexport. So, we need to export.
add_definitions(-Dlibjpeg_EXPORTS)

set(libjpegutils_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/jpegutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iccjpeg.c
    ${DIGIKAM_LIBJPEG_DIR}/transupp.c
)

# no warnings from JPEG extra implementations
if(MSVC)
    set_source_files_properties(${DIGIKAM_LIBJPEG_DIR}/transupp.c PROPERTIES COMPILE_FLAGS /w)
else()
    set_source_files_properties(${DIGIKAM_LIBJPEG_DIR}/transupp.c PROPERTIES COMPILE_FLAGS "-w")
endif()

if(WIN32)
    set(libjpegutils_SRCS
        ${libjpegutils_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/jpegwin.cpp
    )
endif()

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(core_jpegutils_obj OBJECT ${libjpegutils_SRCS})

target_compile_definitions(core_jpegutils_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
