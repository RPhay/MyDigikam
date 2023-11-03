#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if(WIN32)

    if(MINGW)

        # For LibRaw

        find_library(WSOCK32_LIBRARY  wsock32)
        find_library(WS2_32_LIBRARY   ws2_32)

        # For MediaPlayer

        find_library(NETAPI32_LIBRARY netapi32)
        find_library(USEENV_LIBRARY   userenv)
        find_library(PSAPI_LIBRARY    psapi)

    else()  # MSVC

        # For LibRaw

        set(WSOCK32_LIBRARY  wsock32)
        set(WS2_32_LIBRARY   ws2_32)

        # For MediaPlayer

        set(NETAPI32_LIBRARY netapi32)
        set(USEENV_LIBRARY   userenv)
        set(PSAPI_LIBRARY    psapi)

    endif()

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

# Disable indeep warnings from Visual Studio C++

if(MSVC)

    # To disable warnings about no suitable definition provided for explicit template instantiation request.
    add_compile_options(/wd4661)
    # To disable warnings about deprecated POSIX methods().
    add_compile_options(/wd4996)
    # To disable warnings about qualifier applied to reference type ignored.
    add_compile_options(/wd4181)
    # To disable warnings about 'storage-class-keyword' no longer supported as storage class.
    add_compile_options(/wd5033)
    # To disable warnings about size_t to uint conversion data lost.
    add_compile_options(/wd4267)
    # To disable warnings about truncation from double to float.
    add_compile_options(/wd4305)
    # To disable warnings about object specified multiple times.
    add_compile_options(/wd4197)

endif()
