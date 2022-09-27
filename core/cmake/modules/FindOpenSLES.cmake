# Try to find the OpenSLES includes and libraries
# Once done this will define
#
#  OPENSLES_FOUND       - System has the OpenSLES library.
#  OPENSLES_INCLUDE_DIR - The OpenSLES include directory.
#  OPENSLES_LIBRARIES   - The OpenSLES libraries.
#
# SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

get_property(_FIND_LIBRARY_USE_LIB64_PATHS GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS 1)

find_path(OpenSLES_INCLUDE_DIR SLES/OpenSLES.h)

find_library(OpenSLES_LIBRARY NAMES OpenSLES)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OpenSLES DEFAULT_MSG
                                  OpenSLES_INCLUDE_DIR
                                  OpenSLES_LIBRARY)

if(OpenSLES_FOUND)

    set(OPENSLES_FOUND        ${OpenSLES_FOUND})
    set(OPENSLES_INCLUDE_DIRS ${OpenSLES_INCLUDE_DIR})
    set(OPENSLES_LIBRARIES    ${OpenSLES_LIBRARY})

else()

    if(OpenSLES_FIND_REQUIRED)

        message(FATAL_ERROR "Could NOT find OPENSLES")

    endif()

endif()

mark_as_advanced(OPENSLES_INCLUDE_DIR OPENSLES_LIBRARY)

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ${_FIND_LIBRARY_USE_LIB64_PATHS})
