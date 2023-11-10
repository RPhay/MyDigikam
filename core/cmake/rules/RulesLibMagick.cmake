#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find_package(ImageMagick COMPONENTS Magick++
                                    MagickCore
                                    MagickWand)

if(ImageMagick_FOUND)

    set(Magick_FOUND TRUE)
    set(Magick_INCLUDE_DIRS ${ImageMagick_INCLUDE_DIRS})
    set(Magick_LIBRARIES ${ImageMagick_LIBRARIES})
    include_directories(${Magick_INCLUDE_DIRS})

    if(ImageMagick_Magick++_FOUND)

        set(Magick++_DEFINITIONS ${ImageMagick_Magick++_DEFINITIONS})
        add_definitions(${Magick++_DEFINITIONS})

    endif()

else()

    # Fail back to GraphicsMagick if available

    # Try PKGConfig in first.

    pkg_check_modules(MAGICK_CORE GraphicsMagick)
    pkg_check_modules(MAGICK_PLUS GraphicsMagick++)
    pkg_check_modules(MAGICK_WAND GraphicsMagickWand)

    # Basic search path in second.

    # Core include directories rules.

    find_path(GraphicsMagickCore_INCLUDE_DIRS
              NAMES magick/api.h
              HINTS ${MAGICK_CORE_PKGCONF_INCLUDE_DIRS}
              PATH_SUFFIXES GraphicsMagick
    )

    # Core libraries rules.

    find_library(GraphicsMagickCore_LIBRARIES
                 NAMES GraphicsMagick
                 HINTS ${MAGICK_CORE_PKGCONF_LIBRARIES}
    )

    # C++ include directories rules.

    find_path(GraphicsMagick++_INCLUDE_DIRS
              NAMES Magick++
              HINTS ${MAGICK_PLUS_PKGCONF_INCLUDE_DIRS}
              PATH_SUFFIXES GraphicsMagick
    )

    # C++ libraries rules.

    find_library(GraphicsMagick++_LIBRARIES
                 NAMES Magick++
                 HINTS ${MAGICK_PLUS_PKGCONF_LIBRARIES}
    )

    # Wand include directories rules.

    find_path(GraphicsMagickWand_INCLUDE_DIRS
              NAMES wand/magick_wand.h
              HINTS ${MAGICK_WAND_PKGCONF_INCLUDE_DIRS}
              PATH_SUFFIXES GraphicsMagick
    )

    # Wand libraries rules.

    find_library(GraphicsMagickWand_LIBRARIES
                 NAMES Wand
                 HINTS ${MAGICK_WAND_PKGCONF_LIBRARIES}
    )

    message(STATUS "GraphicsMagickCore includes   : ${GraphicsMagickCore_INCLUDE_DIRS}")
    message(STATUS "GraphicsMagickCore libraries  : ${GraphicsMagickCore_LIBRARIES}")
    message(STATUS "GraphicsMagickCore definitions: ${MAGICK_CORE_PKGCONF_CFLAGS_OTHER}")
    message(STATUS "GraphicsMagick++ includes     : ${GraphicsMagick++_INCLUDE_DIRS}")
    message(STATUS "GraphicsMagick++ libraries    : ${GraphicsMagick++_LIBRARIES}")
    message(STATUS "GraphicsMagick++ definitions  : ${MAGICK_PLUS_PKGCONF_CFLAGS_OTHER}")
    message(STATUS "GraphicsMagickWand includes   : ${GraphicsMagickWand_INCLUDE_DIRS}")
    message(STATUS "GraphicsMagickWand libraries  : ${GraphicsMagickWand_LIBRARIES}")
    message(STATUS "GraphicsMagickWand definitions: ${MAGICK_WAND_PKGCONF_CFLAGS_OTHER}")

    if(GraphicsMagickCore_INCLUDE_DIRS AND GraphicsMagickCore_LIBRARIES)
        set(GraphicsMagickCore_FOUND TRUE)
    endif()

    if(GraphicsMagick++_INCLUDE_DIRS AND GraphicsMagick++_LIBRARIES)
        set(GraphicsMagick++_FOUND TRUE)
    endif()

    if(GraphicsMagickWand_INCLUDE_DIRS AND GraphicsMagickWand_LIBRARIES)
        set(GraphicsMagickWand_FOUND TRUE)
    endif()

    if(MSVC AND GraphicsMagickCore_FOUND AND GraphicsMagick++_INCLUDE_DIRS AND GraphicsMagickWand_INCLUDE_DIRS)
        # VCPKG group all libraries in one file.
        set(GraphicsMagick_FOUND TRUE)
        set(GraphicsMagick++_LIBRARIES ${GraphicsMagickCore_LIBRARIES})
        set(GraphicsMagickWand_LIBRARIES ${GraphicsMagickCore_LIBRARIES})
    elseif(GraphicsMagickCore_FOUND AND GraphicsMagick++_FOUND AND GraphicsMagickWand_FOUND)
        # Other tool-chain must provide 3 files.
        set(GraphicsMagick_FOUND TRUE)
    else()
        set(GraphicsMagick_FOUND FALSE)
    endif()

    if(GraphicsMagick_FOUND)

        message(STATUS "GraphicsMagick API used instead ImageMagick")

        # Populate ImageMagick variables

        set(Magick_FOUND TRUE)

        set(Magick_INCLUDE_DIRS ${GraphicsMagickCore_INCLUDE_DIRS} ${GraphicsMagick++_INCLUDE_DIRS} ${GraphicsMagickWand_INCLUDE_DIRS})
        include_directories(${Magick_INCLUDE_DIRS})

        set(Magick_LIBRARIES ${GraphicsMagickCore_LIBRARIES} ${GraphicsMagick++_LIBRARIES} ${GraphicsMagickWand_LIBRARIES})

        set(Magick++_DEFINITIONS ${MAGICK_PLUS_PKGCONF_CFLAGS_OTHER})
        add_definitions(${Magick++_DEFINITIONS})

    endif()

endif()
