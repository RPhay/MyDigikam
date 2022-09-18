#
# - Find the pulseaudio include file and library
#
#  PULSEAUDIO_FOUND        - system has pulseaudio
#  PULSEAUDIO_INCLUDE_DIRS - the pulseaudio include directory
#  PULSEAUDIO_LIBRARIES    - The libraries needed to use pulseaudio
#
# SPDX-FileCopyrightText: 2014 Belledonne Communications, Grenoble France
#
# SPDX-License-Identifier: BSD-3-Clause
#

include(CheckSymbolExists)
include(CMakePushCheckState)

set(_PULSEAUDIO_ROOT_PATHS
    ${CMAKE_INSTALL_PREFIX}
)

find_path(PULSEAUDIO_INCLUDE_DIRS
    NAMES pulse/pulseaudio.h
    HINTS _PULSEAUDIO_ROOT_PATHS
    PATH_SUFFIXES include
)

if(PULSEAUDIO_INCLUDE_DIRS)
    set(HAVE_PULSE_PULSEAUDIO_H 1)
endif()

find_library(PULSEAUDIO_LIBRARIES
    NAMES pulse
    HINTS _PULSEAUDIO_ROOT_PATHS
    PATH_SUFFIXES bin lib
)

if(PULSEAUDIO_LIBRARIES)
    cmake_push_check_state(RESET)
    list(APPEND CMAKE_REQUIRED_INCLUDES ${PULSEAUDIO_INCLUDE_DIRS})
    list(APPEND CMAKE_REQUIRED_LIBRARIES ${PULSEAUDIO_LIBRARIES})
    check_symbol_exists(pa_mainloop_new "pulse/pulseaudio.h" HAVE_PA_MAINLOOP_NEW)
    cmake_pop_check_state()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(PulseAudio
    DEFAULT_MSG
    PULSEAUDIO_INCLUDE_DIRS PULSEAUDIO_LIBRARIES HAVE_PULSE_PULSEAUDIO_H HAVE_PA_MAINLOOP_NEW
)

mark_as_advanced(PULSEAUDIO_INCLUDE_DIRS PULSEAUDIO_LIBRARIES HAVE_PULSE_PULSEAUDIO_H HAVE_PA_MAINLOOP_NEW)

