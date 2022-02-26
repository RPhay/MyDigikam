#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(ImageMagick COMPONENTS Magick++
                                    MagickCore
                                    MagickWand)

if(ImageMagick_Magick++_FOUND)

    add_definitions(${ImageMagick_Magick++_DEFINITIONS})

endif()

if(ImageMagick_FOUND)

    include_directories(${ImageMagick_INCLUDE_DIRS})

endif()
