#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find_package(ImageMagick COMPONENTS Magick++
                                    MagickCore
                                    MagickWand)

if(ImageMagick_Magick++_FOUND)

    add_definitions(${ImageMagick_Magick++_DEFINITIONS})

endif()

if(ImageMagick_FOUND)

    include_directories(${ImageMagick_INCLUDE_DIRS})

else()

    # Fail back to GraphicsMagick if available

    pkg_check_modules(MAGICK_PLUS GraphicsMagick++)
    pkg_check_modules(MAGICK_CORE GraphicsMagick)
    pkg_check_modules(MAGICK_WAND GraphicsMagickWand)

    if(MAGICK_PLUS_FOUND AND MAGICK_CORE_FOUND AND MAGICK_WAND_FOUND)

        message(STATUS "GraphicsMagick API used instead ImageMagick")
        set(ImageMagick_FOUND TRUE)

        message(STATUS "GraphicsMagick definitions: ${MAGICK_PLUS_DEFINITIONS}")
        add_definitions(${MAGICK_PLUS_DEFINITIONS})

        message(STATUS "GraphicsMagick includes: ${MAGICK_CORE_DEFINITIONS}")
        include_directories(${MAGICK_CORE_INCLUDE_DIRS})

    endif()

#   ImageMagick_FOUND                  - TRUE if all components are found.
#   ImageMagick_EXECUTABLE_DIR         - Full path to executables directory.
#   ImageMagick_<component>_FOUND      - TRUE if <component> is found.
#   ImageMagick_<component>_EXECUTABLE - Full path to <component> executable.
#   ImageMagick_VERSION_STRING         - the version of ImageMagick found


endif()
