#
# Copyright (c) 2015,      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
# Copyright (c) 2010-2020, by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(exiftoolcommon_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/common/exiftooltranslator.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/exiftooltranslator_p.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/exiftooltranslator_exif.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/exiftooltranslator_iptc.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/exiftooltranslator_xmp.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/exiftooltranslator_ignoredgroups.cpp
)

add_library(exiftoolcommon STATIC ${exiftoolcommon_SRCS})

#------------------------------------------------------------------------

#add_subdirectory(cpp)
add_subdirectory(qt)
