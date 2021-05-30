#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(${CMAKE_SOURCE_DIR}/core/cmake/modules/DNGSdkRules.cmake)

# =======================================================
# dng validate command line tool from DNG SDK

set(dngvalidate_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/dngwriter/extra/dng_sdk/dng_validate.cpp)

add_executable(dngvalidate_cli ${dngvalidate_cli_SRCS})
ecm_mark_nongui_executable(dngvalidate_cli)

target_link_libraries(dngvalidate_cli

                      libdng
                      digikamcore

                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      PNG::PNG                          # For zlib
                      ${JPEG_LIBRARIES}
                     )

# =======================================================
# dng info command line tool

set(dnginfo_cli_SRCS dnginfo_cli.cpp)

add_executable(dnginfo_cli ${dnginfo_cli_SRCS})
ecm_mark_nongui_executable(dnginfo_cli)

target_link_libraries(dnginfo_cli

                      libdng
                      digikamcore

                      ${COMMON_TEST_LINK}

                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      PNG::PNG                          # For zlib
                      ${JPEG_LIBRARIES}
                     )

# =======================================================
# dng compare command line tool

set(dngcompare_cli_SRCS dngcompare_cli.cpp)

add_executable(dngcompare_cli ${dngcompare_cli_SRCS})
ecm_mark_nongui_executable(dngcompare_cli)

target_link_libraries(dngcompare_cli

                      libdng
                      digikamcore

                      ${COMMON_TEST_LINK}

                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      PNG::PNG                          # For zlib
                      ${JPEG_LIBRARIES}
                     )

# =======================================================
# raw to dng command line tool

set(raw2dng_cli_SRCS raw2dng_cli.cpp)

add_executable(raw2dng_cli ${raw2dng_cli_SRCS})
ecm_mark_nongui_executable(raw2dng_cli)

target_link_libraries(raw2dng_cli

                      libdng
                      digikamcore

                      ${COMMON_TEST_LINK}

                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${TIFF_LIBRARIES}
                      PNG::PNG                          # For zlib
                      ${JPEG_LIBRARIES}
                     )
