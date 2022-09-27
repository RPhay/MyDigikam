# MacroSSE.cmake
#
# CMake module for check whether a SSE instructions can be used on the machine.
#
# - Supports SSE2/SSE3/SSSE3/SSE4.1/SSE4.2
# - Supports check GCC version(-msse4.2 can use on GCC 4.3 or later)
#
# Usage:
#
# include(MacroSSE)
# CheckSSESupport()
# if(SSE3_FOUND)
#     if(SSSE3_FOUND)
#         set(CXX_DFLAGS -msse3 -mssse3)
#     endif()
# endif()
#
# if(SSE4_2_FOUND)
#     set(CXX_DFLAGS -msse4.2 -mpopcnt)
# endif()
# add_definitions(${CXX_DFLAGS})
#
# Variables:
#
# - SSE2_FOUND
# - SSE3_FOUND
# - SSSE3_FOUND
# - SSE4_1_FOUND
# - SSE4_2_FOUND
#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

macro(CheckSSESupport)

    if(CMAKE_SYSTEM_NAME MATCHES "Linux")

        exec_program(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)

        string(REGEX REPLACE "^.*(sse2).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "sse2" "${SSE_THERE}" SSE2_TRUE)

        if(SSE2_TRUE)

            set(SSE2_FOUND true CACHE BOOL "SSE2 available on host")

        else()

            set(SSE2_FOUND false CACHE BOOL "SSE2 available on host")

        endif()

        # /proc/cpuinfo apparently omits sse3

        string(REGEX REPLACE "^.*[^s](sse3).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "sse3" "${SSE_THERE}" SSE3_TRUE)

        if(NOT SSE3_TRUE)

            string(REGEX REPLACE "^.*(T2300).*$" "\\1" SSE_THERE ${CPUINFO})
            string(COMPARE EQUAL "T2300" "${SSE_THERE}" SSE3_TRUE)

        endif()

        string(REGEX REPLACE "^.*(ssse3).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "ssse3" "${SSE_THERE}" SSSE3_TRUE)

        if(SSE3_TRUE OR SSSE3_TRUE)

            set(SSE3_FOUND true CACHE BOOL "SSE3 available on host")

        else()

            set(SSE3_FOUND false CACHE BOOL "SSE3 available on host")

        endif()

        if(SSSE3_TRUE)

            set(SSSE3_FOUND true CACHE BOOL "SSSE3 available on host")

        else()

            set(SSSE3_FOUND false CACHE BOOL "SSSE3 available on host")

        endif()

        string(REGEX REPLACE "^.*(sse4_1).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "sse4_1" "${SSE_THERE}" SSE41_TRUE)

        if(SSE41_TRUE)

            set(SSE4_1_FOUND true CACHE BOOL "SSE4.1 available on host")

        else()

            set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")

        endif()

        string(REGEX REPLACE "^.*(sse4_2).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "sse4_2" "${SSE_THERE}" SSE42_TRUE)

        if(SSE42_TRUE)

            set(SSE4_2_FOUND true CACHE BOOL "SSE4.2 available on host")

        else()

            set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")

        endif()

    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")

        exec_program("/usr/sbin/sysctl -n machdep.cpu.features" OUTPUT_VARIABLE CPUINFO)

        string(REGEX REPLACE "^.*[^S](SSE2).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "SSE2" "${SSE_THERE}" SSE2_TRUE)

        if(SSE2_TRUE)

            set(SSE2_FOUND true CACHE BOOL "SSE2 available on host")

        else()

            set(SSE2_FOUND false CACHE BOOL "SSE2 available on host")

        endif()

        string(REGEX REPLACE "^.*[^S](SSE3).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "SSE3" "${SSE_THERE}" SSE3_TRUE)

        if(SSE3_TRUE)

            set(SSE3_FOUND true CACHE BOOL "SSE3 available on host")

        else()

            set(SSE3_FOUND false CACHE BOOL "SSE3 available on host")

        endif()

        string(REGEX REPLACE "^.*(SSSE3).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "SSSE3" "${SSE_THERE}" SSSE3_TRUE)

        if(SSSE3_TRUE)

            set(SSSE3_FOUND true CACHE BOOL "SSSE3 available on host")

        else()

            set(SSSE3_FOUND false CACHE BOOL "SSSE3 available on host")

        endif()

        string(REGEX REPLACE "^.*(SSE4.1).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "SSE4.1" "${SSE_THERE}" SSE41_TRUE)

        if(SSE41_TRUE)

            set(SSE4_1_FOUND true CACHE BOOL "SSE4.1 available on host")

        else()

            set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")

        endif()

        string(REGEX REPLACE "^.*(SSE4.2).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "SSE4.2" "${SSE_THERE}" SSE42_TRUE)

        if(SSE42_TRUE)

            set(SSE4_2_FOUND true CACHE BOOL "SSE4.2 available on host")

        else()

            set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")

        endif()

    elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")

        cmake_host_system_information(RESULT SSE2_FOUND QUERY HAS_SSE2)

        # TODO: Add more SSE support

        set(SSE2_FOUND   true  CACHE BOOL "SSE2 available on host")
        set(SSE3_FOUND   false CACHE BOOL "SSE3 available on host")
        set(SSSE3_FOUND  false CACHE BOOL "SSSE3 available on host")
        set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
        set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")

    else()

        cmake_host_system_information(RESULT SSE2_FOUND QUERY HAS_SSE2)

        set(SSE2_FOUND   true  CACHE BOOL "SSE2 available on host")
        set(SSE3_FOUND   false CACHE BOOL "SSE3 available on host")
        set(SSSE3_FOUND  false CACHE BOOL "SSSE3 available on host")
        set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
        set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")

    endif()

    if(CMAKE_COMPILER_IS_GNUCXX)

        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

        if(GCC_VERSION VERSION_LESS 4.2)

            set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host" FORCE)
            set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host" FORCE)

        endif()

    endif()

    message(STATUS "SSE2 support   : ${SSE2_FOUND}")
    message(STATUS "SSE3 support   : ${SSE3_FOUND}")
    message(STATUS "SSSE3 support  : ${SSSE3_FOUND}")
    message(STATUS "SSE4.1 support : ${SSE4_1_FOUND}")
    message(STATUS "SSE4.2 support : ${SSE4_2_FOUND}")

    mark_as_advanced(SSE2_FOUND SSE3_FOUND SSSE3_FOUND SSE4_1_FOUND SSE4_2_FOUND)

endmacro()
