#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(${CMAKE_SOURCE_DIR}/core/cmake/modules/DNGSdkRules.cmake)

# =======================================================
# DNGVALIDATE tool from DNG SDK

set(dngvalidate_SRCS ${CMAKE_SOURCE_DIR}/core/libs/dngwriter/extra/dng_sdk/dng_validate.cpp)

add_executable(dngvalidate ${dngvalidate_SRCS})
ecm_mark_nongui_executable(dngvalidate)

target_link_libraries(dngvalidate

                      libdng
                      digikamcore

                      ${COMMON_TEST_LINK}
)

# =======================================================
# DNGINFO command line tool

set(dnginfo_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/dnginfo.cpp)

add_executable(dnginfo ${dnginfo_SRCS})
ecm_mark_nongui_executable(dnginfo)

target_link_libraries(dnginfo

                      libdng
                      digikamcore

                      ${COMMON_TEST_LINK}
)

# =======================================================
# RAW2DNG command line tool

set(raw2dng_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/raw2dng.cpp)

add_executable(raw2dng ${raw2dng_SRCS})

ecm_mark_nongui_executable(raw2dng)

target_link_libraries(raw2dng

                      digikamcore

                      ${COMMON_TEST_LINK}
)
