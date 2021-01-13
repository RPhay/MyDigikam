# Macro to create build date header
#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

macro(BUILD_DATE_HEADER)

    # Add a custom command to drive the build date script

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/builddatescript.cmake.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/builddatescript.cmake"
                   @ONLY)

    # Add a custom target to drive the custom command.

    add_custom_target(digikam-builddate ALL COMMAND ${CMAKE_COMMAND} -P
                      "${CMAKE_CURRENT_BINARY_DIR}/builddatescript.cmake")

endmacro()
