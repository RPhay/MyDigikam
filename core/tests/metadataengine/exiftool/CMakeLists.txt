#
# Copyright (c) 2015,      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
# Copyright (c) 2010-2021, by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
)

macro(EXIFTOOL_BUILD_CLITEST)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)

    set(${_target}_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/${_filename})

    add_executable(${_target} ${${_target}_SRCS})
    ecm_mark_nongui_executable(${_target})

    target_link_libraries(${_target}

                          digikamcore

                          ${COMMON_TEST_LINK}
    )

endmacro()

#------------------------------------------------------------------------

EXIFTOOL_BUILD_CLITEST(exiftoolexport_cli.cpp)
EXIFTOOL_BUILD_CLITEST(exiftooloutput_cli.cpp)
EXIFTOOL_BUILD_CLITEST(exiftoolmulticore_cli.cpp)
EXIFTOOL_BUILD_CLITEST(exiftoolparserout_cli.cpp)
EXIFTOOL_BUILD_CLITEST(exiftoolwrite_cli.cpp)
EXIFTOOL_BUILD_CLITEST(exiftoolformats_cli.cpp)
EXIFTOOL_BUILD_CLITEST(exiftoollanguages_cli.cpp)
EXIFTOOL_BUILD_CLITEST(exiftoolapplychanges_cli.cpp)
