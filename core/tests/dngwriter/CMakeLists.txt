#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(${CMAKE_SOURCE_DIR}/core/cmake/modules/DNGSdkRules.cmake)

# =======================================================
# DNGVALIDATE tool from DNG SDK

set(dngvalidate_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/dngwriter/extra/dng_sdk/dng_validate.cpp)

add_executable(dngvalidate ${dngvalidate_SRCS})
ecm_mark_nongui_executable(dngvalidate)

target_link_libraries(dngvalidate

                      libdng
                      digikamcore

                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      PNG::PNG                          # For zlib
                      ${JPEG_LIBRARIES}
                     )

# =======================================================
# DNGINFO command line tool

set(dnginfo_SRCS dnginfo.cpp)

add_executable(dnginfo ${dnginfo_SRCS})
ecm_mark_nongui_executable(dnginfo)

target_link_libraries(dnginfo

                      libdng
                      digikamcore

                      ${COMMON_TEST_LINK}

                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      PNG::PNG                          # For zlib
                      ${JPEG_LIBRARIES}
                     )

# =======================================================
# RAW2DNG command line tool

set(raw2dng_SRCS raw2dng.cpp)

add_executable(raw2dng
               ${raw2dng_SRCS}
              )

ecm_mark_nongui_executable(raw2dng)

target_link_libraries(raw2dng

                      libdng
                      digikamcore

                      ${COMMON_TEST_LINK}

                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${TIFF_LIBRARIES}
                      PNG::PNG                          # For zlib
                      ${JPEG_LIBRARIES}
                     )
