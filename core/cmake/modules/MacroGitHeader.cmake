# Macro to create git version header
#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

macro(GIT_HEADER)

    # We only do this IF we are in a .git dir

    find_file(GIT_MARKER entries PATHS ${CMAKE_SOURCE_DIR}/.git)

    if(NOT GIT_MARKER)

        set(GIT_MARKER ${CMAKE_SOURCE_DIR}/CMakeLists.txt)  # Dummy file

    endif()

    # Add a custom command to drive the git script whenever the git entries
    # file changes.

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/gitscript.cmake.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/gitscript.cmake"
                   @ONLY)

    # Add a custom target to drive the custom command.

    add_custom_target(digikam-gitversion ALL COMMAND ${CMAKE_COMMAND} -P
                      "${CMAKE_CURRENT_BINARY_DIR}/gitscript.cmake")

endmacro()
