#
# Copyright (c) 2017-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libtransitionmngr_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/effectpreview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/effectmngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/effectmngr_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/effectmngr_p_pan.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/effectmngr_p_zoom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionpreview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr_p_abstract.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr_p_blur.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr_p_lines.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr_p_push.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr_p_slide.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr_p_squares.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transitionmngr_p_swap.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamcore
add_library(core_libtransitionmngr_obj OBJECT ${libtransitionmngr_SRCS})

target_compile_definitions(core_libtransitionmngr_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
