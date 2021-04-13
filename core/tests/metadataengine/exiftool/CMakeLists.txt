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

set(libexiftool_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolcore.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolpipe.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftooltaginfo.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftooltranslator.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftooltranslator_p.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftooltranslator_exif.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftooltranslator_iptc.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftooltranslator_xmp.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/exiftooltranslator_ignoredgroups.cpp
)

add_library(libexiftool STATIC ${libexiftool_SRCS})

#------------------------------------------------------------------------

set(exiftoolloader_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolloader_cli.cpp)
add_executable(exiftoolloader_cli ${exiftoolloader_cli_SRCS})
ecm_mark_nongui_executable(exiftoolloader_cli)

target_link_libraries(exiftoolloader_cli
                      libexiftool
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(exiftoolexport_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolexport_cli.cpp)
add_executable(exiftoolexport_cli ${exiftoolexport_cli_SRCS})
ecm_mark_nongui_executable(exiftoolexport_cli)

target_link_libraries(exiftoolexport_cli
                      libexiftool
                      digikamcore

                      ${COMMON_TEST_LINK}
)

