#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
)

set(libvideotools_SRCS
    manager/vidslidesettings.cpp
    manager/vidslidethread.cpp
    manager/vidslidetask.cpp

    player/vidplayerdlg.cpp
    player/mediaplayerview.cpp
    player/slidevideo.cpp
)

add_library(videotools_src OBJECT ${libvideotools_SRCS})

target_compile_definitions(videotools_src
                           PRIVATE
                           digikamcore_EXPORTS
)
