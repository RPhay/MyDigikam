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

set(exiftoolloader_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolloader_cli.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/ExifTool.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/ExifToolPipe.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/TagInfo.cpp
)

add_executable(exiftoolloader_cli ${exiftoolloader_cli_SRCS})
ecm_mark_nongui_executable(exiftoolloader_cli)

target_link_libraries(exiftoolloader_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

