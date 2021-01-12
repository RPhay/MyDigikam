#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# Set CMake variable BUILD_TIME to 'now'. This 'now' is the time
# when CMake is run, not when the target builder (e.g. make) is run.

string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M:%S" UTC)

include_directories($<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>)

# Used by digikamcore
add_library(core_dbuilddate_obj OBJECT dbuilddate.cpp)

target_compile_definitions(core_dbuilddate_obj
                           PRIVATE
                           digikamcore_EXPORTS)

target_compile_definitions(core_dbuilddate_obj
                           PRIVATE
                           -DBUILD_TIME="${BUILD_TIME}")

# Add a dummy target that removes the CMake variable BUILD_TIME from CMake's cache.
# this forces CMake to be-rerun when we hit this target.

add_custom_target(clear_cache
                  COMMAND ${CMAKE_COMMAND} -U BUILD_TIME ${CMAKE_BINARY_DIR})

# Have the cache clearing be run before trying to build the dbuilddate object.
# This is the same as a PRE_BUILD custom_command. But PRE_BUILD is
# available for VS generators only, on others it is synonymous to PRE_LINK,
# i.e. "post compile"

add_dependencies(core_dbuilddate_obj clear_cache)
