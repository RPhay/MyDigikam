# - Try to find HEIF library
# Once done this will define
#
# Heif_FOUND            True if HEIF library was found.
# HEIF_INCLUDES_DIR     Where to find HEIF library headers
# HEIF_LIBRARIES        List of libraries to link against when using HEIF library
# HEIF_VERSION          Version of HEIF library (e.g., 3.6.2)
#
#  Copyright (c) 2019-2021 Caulier Gilles <caulier dot gilles at gmail dot com>
#
#  Redistribution and use is allowed according to the terms of the New BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if(HEIF_LIBRARIES AND HEIF_INCLUDE_DIRS AND HEIF_VERSION)

    # in cache already
    Set(HEIF_FOUND TRUE)

else()

    include(FindPackageHandleStandardArgs)

    find_path(HEIF_INCLUDES_DIR libheif/heif_version.h
              HINTS             ${HEIF_INCLUDE_PATH}
              ENV               HEIF_INCLUDE_PATH
              DOC               "The directory where libheif headers reside"
    )

    find_library(HEIF_LIBRARIES heif
                 HINTS          ${HEIF_LIBRARY_PATH}
                 ENV            HEIF_LIBRARY_PATH
                 DOC            "The directory where libheif libraries reside"
    )

    if(HEIF_INCLUDES_DIR)

        file(STRINGS "${HEIF_INCLUDES_DIR}/libheif/heif_version.h" TMP REGEX "^#define LIBHEIF_VERSION[ \t].*$")
        string(REGEX MATCHALL "[0-9.]+" HEIF_VERSION ${TMP})

    endif()

    find_package_handle_standard_args(Libheif
                                      REQUIRED_VARS HEIF_INCLUDES_DIR
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

    mark_as_advanced(HEIF_INCLUDES_DIR HEIF_LIBRARIES HEIF_VERSION)

    message(STATUS "HEIF_FOUND        = ${HEIF_FOUND}")
    message(STATUS "HEIF_INCLUDES_DIR = ${HEIF_INCLUDES_DIR}")
    message(STATUS "HEIF_LIBRARIES    = ${HEIF_LIBRARIES}")
    message(STATUS "HEIF_VERSION      = ${HEIF_VERSION}")

endif()
