#
# Copyright (c) 2010-2020, by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(exiftoolqt_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/qexiftoolprocess.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/exiftoolparser.cpp
)

add_library(exiftoolqt STATIC ${exiftoolqt_SRCS})

#------------------------------------------------------------------------

set(qexiftoolexport_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/qexiftoolexport_cli.cpp)
add_executable(qexiftoolexport_cli ${qexiftoolexport_cli_SRCS})
ecm_mark_nongui_executable(qexiftoolexport_cli)

target_link_libraries(qexiftoolexport_cli
                      exiftoolqt
                      exiftoolcommon
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(qexiftoolloader_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/qexiftoolloader_cli.cpp)
add_executable(qexiftoolloader_cli ${qexiftoolloader_cli_SRCS})
ecm_mark_nongui_executable(qexiftoolloader_cli)

target_link_libraries(qexiftoolloader_cli
                      exiftoolqt
                      exiftoolcommon
                      digikamcore

                      ${COMMON_TEST_LINK}
)

