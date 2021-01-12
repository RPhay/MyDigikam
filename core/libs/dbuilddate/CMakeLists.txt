#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# Set CMake variable BUILD_TIME to 'now'. This 'now' is the time
# when CMake is run, not when the target builder (e.g. make) is run.

string(TIMESTAMP BUILD_DATE "%Y-%m-%dT%H:%M:%S" UTC)

# Generate text file with build time stamp for bundles workflow.

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BUILD_DATE.cmake.in
               ${CMAKE_BINARY_DIR}/BUILD_DATE)

include_directories($<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>)

# Used by digikamcore
add_library(core_dbuilddate_obj OBJECT dbuilddate.cpp)

target_compile_definitions(core_dbuilddate_obj
                           PRIVATE
                           digikamcore_EXPORTS)

target_compile_definitions(core_dbuilddate_obj
                           PRIVATE
                           -DBUILD_DATE="${BUILD_DATE}")

# Add a dummy target that removes the CMake variable BUILD_TIME from CMake's cache.
# this forces CMake to be-rerun when we hit this target.

add_custom_target(clear_cache
                  COMMAND ${CMAKE_COMMAND} -U BUILD_DATE ${CMAKE_BINARY_DIR})

# Have the cache clearing be run before trying to build the dbuilddate object.
# This is the same as a PRE_BUILD custom_command. But PRE_BUILD is
# available for VS generators only, on others it is synonymous to PRE_LINK,
# i.e. "post compile"

add_dependencies(core_dbuilddate_obj clear_cache)
