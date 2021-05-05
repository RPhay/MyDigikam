#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)

add_subdirectory(alignment)
add_subdirectory(preprocess)
add_subdirectory(detection)
add_subdirectory(recognition)
add_subdirectory(demo)
add_subdirectory(helper)
