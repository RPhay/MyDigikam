#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_subdirectory(converttoavif)
add_subdirectory(converttodng)
add_subdirectory(converttojpeg)
add_subdirectory(converttojxl)
add_subdirectory(converttopng)
add_subdirectory(converttopgf)
add_subdirectory(converttotiff)
add_subdirectory(converttowebp)

if(Jasper_FOUND)
    add_subdirectory(converttojp2)
endif()

if(X265_FOUND)
    add_subdirectory(converttoheif)
endif()
