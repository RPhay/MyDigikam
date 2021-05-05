# An useful macro to manage sanitizer rules
#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

macro(ENABLE_COMPILER_SANITIZERS)

    if(ENABLE_SANITIZERS)

        find_library(ASAN_LIBRARIES NAMES asan)

        if (ASAN_LIBRARIES)

            message(STATUS "Found AddressSanitizer library: ${ASAN_LIBRARIES}")
            set(ASAN_FOUND TRUE CACHE INTERNAL "")
            set(ECM_ENABLE_SANITIZERS ${ECM_ENABLE_SANITIZERS} "address")

        else()

            message(STATUS "Could not find AddressSanitizer library. Please install libasan...")

        endif()

        ###

        find_library(LSAN_LIBRARIES NAMES lsan)

        if (LSAN_LIBRARIES)

            message(STATUS "Found LeakSanitizer library: ${LSAN_LIBRARIES}")
            set(LSAN_FOUND TRUE CACHE INTERNAL "")
            set(ECM_ENABLE_SANITIZERS ${ECM_ENABLE_SANITIZERS} "leak")

        else()

            message(STATUS "Could not find LeakSanitizer library. Please install liblsan...")

        endif()

        ###

        find_library(UBSAN_LIBRARIES NAMES ubsan)

        if (UBSAN_LIBRARIES)

            message(STATUS "Found UndefinedBehaviorSanitizer library: ${UBSAN_LIBRARIES}")
            set(UBSAN_FOUND TRUE CACHE INTERNAL "")
            set(ECM_ENABLE_SANITIZERS ${ECM_ENABLE_SANITIZERS} "undefined")

        else()

            message(STATUS "Could not find UndefinedBehaviorSanitizer library. Please install libubsan...")

        endif()

        ###

        find_library(MSAN_LIBRARIES NAMES msan)

        if (MSAN_LIBRARIES)

            message(STATUS "Found MemorySanitizer library: ${MSAN_LIBRARIES}")
            set(MSAN_FOUND TRUE CACHE INTERNAL "")
            set(ECM_ENABLE_SANITIZERS ${ECM_ENABLE_SANITIZERS} "memory")

        else()

            message(STATUS "Could not find MemorySanitizer library. Please install libmsan...")

        endif()

        ###

        find_library(MSAN_LIBRARIES NAMES tsan)

        if (TSAN_LIBRARIES)

            message(STATUS "Found ThreadSanitizer library: ${TSAN_LIBRARIES}")
            set(TSAN_FOUND TRUE CACHE INTERNAL "")
            set(ECM_ENABLE_SANITIZERS ${ECM_ENABLE_SANITIZERS} "thread")

        else()

            message(STATUS "Could not find ThreadSanitizer library. Please install libtsan...")

        endif()

        ###

        message(STATUS "Building with sanitizers: ${ECM_ENABLE_SANITIZERS}")

        if ("${ECM_ENABLE_SANITIZERS}" STREQUAL "")

            message(FATAL_ERROR "Could not find Sanitizer libraries")

        endif()

        include(ECMEnableSanitizers)

    endif()

endmacro()

