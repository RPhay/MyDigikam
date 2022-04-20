# Finds the XAudio2 library (https://en.wikipedia.org/wiki/XAudio2)
#
# This will define the following variables:
#
#  XAUDIO2_FOUND        - system has XAudio2
#  XAUDIO2_INCLUDE_DIRS
#  XAUDIO2_LIBRARIES
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if(WIN32)

    find_path(XAUDIO2_INCLUDE_DIRS xaudio2.h PATHS
        "$ENV{DXSDK_DIR}/Include"
        "$ENV{PROGRAMFILES}/Microsoft DirectX SDK*/Include"
    )

    get_filename_component(DirectX_ROOT_DIR "${DirectX_INCLUDE_DIRS}/.." ABSOLUTE)

    if(CMAKE_CL_64)

        set(DirectX_LIBRARY_PATHS "${DirectX_ROOT_DIR}/Lib/x64")

    else()

        set(DirectX_LIBRARY_PATHS "${DirectX_ROOT_DIR}/Lib/x86" "${DirectX_ROOT_DIR}/Lib")

    endif()

    find_library(DirectX_X3DAUDIO_LIBRARY X3DAudio ${DirectX_LIBRARY_PATHS} NO_DEFAULT_PATH)

    find_library(DirectX_XAPOFX_LIBRARY xapofx     ${DirectX_LIBRARY_PATHS} NO_DEFAULT_PATH)

    set(XAUDIO2_LIBRARIES ${DirectX_X3DAUDIO_LIBRARY} ${DirectX_XAPOFX_LIBRARY})

    include(FindPackageHandleStandardArgs)

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(XAUDIO2
                                      DEFAULT_MSG
                                          DirectX_ROOT_DIR
                                          XAUDIO2_LIBRARIES
                                          XAUDIO2_INCLUDE_DIRS
    )

    mark_as_advanced(XAUDIO2_INCLUDE_DIRS
                     XAUDIO2_LIBRARIES
    )

endif()
