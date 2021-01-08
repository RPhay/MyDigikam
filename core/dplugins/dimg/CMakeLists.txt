#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(jpeg)
add_subdirectory(png)
add_subdirectory(tiff)
add_subdirectory(pgf)
add_subdirectory(raw)
add_subdirectory(qimage)
add_subdirectory(heif)

if(Jasper_FOUND)
    add_subdirectory(jpeg2000)
endif()

if(ImageMagick_FOUND)
    add_subdirectory(imagemagick)
endif()
