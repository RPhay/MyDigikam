#
# Copyright (c) 2017-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libtransitionmngr_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/effectpreview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/effectmngr.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/effectmngr_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/effectmngr_p_pan.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/effectmngr_p_zoom.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/frameutils.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionpreview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr_p_abstract.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr_p_blur.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr_p_lines.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr_p_push.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr_p_slide.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr_p_squares.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/transitionmngr/transitionmngr_p_swap.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(digikamlibtransitionmngr_src OBJECT ${libtransitionmngr_SRCS})
