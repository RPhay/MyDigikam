#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_subdirectory(expoblending)
add_subdirectory(calendar)
add_subdirectory(sendbymail)
add_subdirectory(printcreator)
add_subdirectory(jalbum)
add_subdirectory(mediaserver)
add_subdirectory(mjpegstream)
add_subdirectory(wallpaper)
add_subdirectory(dngconverter)      # NOTE: for Showfoto only
add_subdirectory(ocrtextconverter)

if(LibXml2_FOUND AND LibXslt_FOUND)
    add_subdirectory(htmlgallery)
endif()

if(ENABLE_MEDIAPLAYER)
    add_subdirectory(videoslideshow)
endif()

if(FLEX_FOUND AND BISON_FOUND AND KF5ThreadWeaver_FOUND)
    add_subdirectory(panorama)
endif()

