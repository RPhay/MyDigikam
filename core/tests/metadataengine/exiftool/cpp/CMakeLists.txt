#
# Copyright (c) 2010-2020, by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(exiftoolcpp_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolcore.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolpipe.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftooltaginfo.cpp
)

add_library(exiftoolcpp STATIC ${exiftoolcpp_SRCS})

#------------------------------------------------------------------------

set(exiftoolloader_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolloader_cli.cpp)
add_executable(exiftoolloader_cli ${exiftoolloader_cli_SRCS})
ecm_mark_nongui_executable(exiftoolloader_cli)

target_link_libraries(exiftoolloader_cli
                      exiftoolcpp
                      exiftoolcommon
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(exiftoolexport_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolexport_cli.cpp)
add_executable(exiftoolexport_cli ${exiftoolexport_cli_SRCS})
ecm_mark_nongui_executable(exiftoolexport_cli)

target_link_libraries(exiftoolexport_cli
                      exiftoolcpp
                      exiftoolcommon
                      digikamcore

                      ${COMMON_TEST_LINK}
)

