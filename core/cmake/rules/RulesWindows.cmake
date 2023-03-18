#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if(WIN32)

    # For LibRaw

    find_library(WSOCK32_LIBRARY  wsock32)
    find_library(WS2_32_LIBRARY   ws2_32)

    # For MediaPlayer

    find_library(NETAPI32_LIBRARY netapi32)
    find_library(USEENV_LIBRARY   userenv)
    find_library(PSAPI_LIBRARY    psapi)

endif()

if(MINGW)

    # NOTE: cross-compilation rules inspired from https://phabricator.kde.org/T3917

    if(ENABLE_MINGW_HARDENING_LINKER)

        set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--dynamicbase -Wl,--nxcompat -Wl,--disable-auto-image-base")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--dynamicbase -Wl,--nxcompat -Wl,--disable-auto-image-base")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--dynamicbase -Wl,--nxcompat -Wl,--disable-auto-image-base")

        if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

            # Enable high-entropy ASLR for 64-bit
            # The image base has to be >4GB for HEASLR to be enabled.
            # The values used here are kind of arbitrary.

            set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--high-entropy-va -Wl,--image-base,0x140000000")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--high-entropy-va -Wl,--image-base,0x180000000")
            set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--high-entropy-va -Wl,--image-base,0x180000000")

        endif()

        message(STATUS "MinGW linker Security Flags enabled")

    else()

        message(WARNING "MinGW Linker Security Flags not enabled!")

    endif()

endif()

# Increase the stack size to prevent crashes with GMic-qt.

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:16777216")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /STACK:16777216")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /STACK:16777216")
elseif(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,16777216")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--stack,16777216")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--stack,16777216")
endif()
