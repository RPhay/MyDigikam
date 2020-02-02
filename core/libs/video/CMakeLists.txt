#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
)

set(libvideotools_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/video/manager/vidslidesettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/video/manager/vidslidethread.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/video/manager/vidslidetask.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/video/player/vidplayerdlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/video/player/mediaplayerview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/video/player/slidevideo.cpp
)

add_library(videotools_src OBJECT ${libvideotools_SRCS})
