#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libgeomapwrapper_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gpsiteminfo.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/gpsiteminfosorter.cpp)

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(core_geomapwrapper_obj OBJECT ${libgeomapwrapper_SRCS})

target_compile_definitions(core_geomapwrapper_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
