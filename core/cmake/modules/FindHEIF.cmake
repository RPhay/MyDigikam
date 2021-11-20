# - Try to find HEIF library
# Once done this will define
#
# Heif_FOUND            True if HEIF library was found.
# HEIF_INCLUDES_DIR     Where to find HEIF library headers
# HEIF_LIBRARIES        List of libraries to link against when using HEIF library
# HEIF_DEFINITIONS      Compiler switches required for using HEIF library
# HEIF_VERSION          Version of HEIF library (e.g., 3.6.2)
#
#  Copyright (c) 2019-2021 Caulier Gilles <caulier dot gilles at gmail dot com>
#
#  Redistribution and use is allowed according to the terms of the New BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if(HEIF_LIBRARIES AND HEIF_INCLUDE_DIRS AND HEIF_VERSION AND HEIF_DEFINITIONS)

    # in cache already
    Set(HEIF_FOUND TRUE)

else()

    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    if(NOT WIN32)

        find_package(PkgConfig)

        if(PKG_CONFIG_FOUND)

           PKG_CHECK_MODULES(PC_HEIF libheif)

        endif()

    endif()

    set(HEIF_DEFINITIONS ${PC_HEIF_CFLAGS_OTHER})

    include(FindPackageHandleStandardArgs)

    find_path(HEIF_INCLUDES_DIR libheif/heif_version.h
              HINTS             ${PC_HEIF_INCLUDE_DIRS}
    )

    find_library(HEIF_LIBRARIES heif
                 HINTS          ${PC_HEIF_LIBRARY_DIRS}
    )

    if(HEIF_INCLUDES_DIR)

        file(STRINGS "${HEIF_INCLUDES_DIR}/libheif/heif_version.h" TMP REGEX "^#define LIBHEIF_VERSION[ \t].*$")
        string(REGEX MATCHALL "[0-9.]+" HEIF_VERSION ${TMP})

    endif()

    find_package_handle_standard_args(Libheif
                                      REQUIRED_VARS
                                        HEIF_INCLUDES_DIR
                                        HEIF_LIBRARIES
    )

    if(Libheif_FOUND)

        set(HEIF_FOUND ${Libheif_FOUND})

        if(NOT TARGET Libheif::Libheif)

            add_library(Libheif::Libheif UNKNOWN IMPORTED)

            set_target_properties(Libheif::Libheif PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${HEIF_INCLUDES_DIR}")

            set_property(TARGET Libheif::Libheif APPEND PROPERTY
                IMPORTED_LOCATION "${HEIF_LIBRARIES}")

        endif()

    endif()

    mark_as_advanced(HEIF_INCLUDES_DIR HEIF_LIBRARIES HEIF_VERSION HEIF_DEFINITIONS)

    message(STATUS "HEIF_FOUND        = ${HEIF_FOUND}")
    message(STATUS "HEIF_INCLUDES_DIR = ${HEIF_INCLUDES_DIR}")
    message(STATUS "HEIF_LIBRARIES    = ${HEIF_LIBRARIES}")
    message(STATUS "HEIF_DEFINITIONS  = ${HEIF_DEFINITIONS}")
    message(STATUS "HEIF_VERSION      = ${HEIF_VERSION}")

endif()
