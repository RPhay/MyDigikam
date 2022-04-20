# Finds the DirectSound library (https://en.wikipedia.org/wiki/Direct3D)
#
# This will define the following variables:
#
#  DIRECTSOUND_FOUND        - system has direct sound
#  DIRECTSOUND_INCLUDE_DIRS - include directory
#  DIRECTSOUND_LIBRARIES    - Link these to use direct sound
#
# Copyright (c) 2011,      Patrick von Reth, <patrick dot vonreth at gmail dot com>
# Copyright (c) 2008-2011, Peter Grasch, <peter dot grasch at bedahr dot org>
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if(WIN32)

    set(DIRECTSOUND_INCLUDE_DIRS $ENV{DXSDK_DIR}/include $ENV{DXSDK_DIR})

    if(CMAKE_CL_64)

        set(CMAKE_MSVC_ARCH amd64)

    else()

        set(CMAKE_MSVC_ARCH x86)

    endif()

    find_library(_DSOUD_LIB NAMES dsound PATHS
                 $ENV{DXSDK_DIR}/Lib/${CMAKE_MSVC_ARCH})

    find_library(_DXGUID_LIB NAMES dxguid PATHS
                 $ENV{DXSDK_DIR}/Lib/${CMAKE_MSVC_ARCH})

    find_library(_DXERR_LIB NAMES dxerr PATHS
                 $ENV{DXSDK_DIR}/Lib/${CMAKE_MSVC_ARCH})

    set(DIRECTSOUND_LIBRARIES ${_DSOUD_LIB} ${_DXGUID_LIB} ${_DXERR_LIB})

    include(FindPackageHandleStandardArgs)

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(DirectSound DEFAULT_MSG DIRECTSOUND_LIBRARIES)

    set(DIRECTSOUND_FOUND ${DirectSound_FOUND})

    mark_as_advanced(DIRECTSOUND_INCLUDE_DIRS
                     DIRECTSOUND_LIBRARIES
    )

endif()

