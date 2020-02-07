#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libthreadimageio_SRCS
    engine/dfileoperations.cpp
    engine/filereadwritelock.cpp
    engine/managedloadsavethread.cpp
    engine/sharedloadsavethread.cpp

    preview/previewloadthread.cpp
    preview/previewtask.cpp
    preview/previewsettings.cpp

    thumb/thumbnailbasic.cpp
    thumb/thumbnailcreator.cpp
    thumb/thumbnailcreator_freedesktop.cpp
    thumb/thumbnailcreator_database.cpp
    thumb/thumbnailcreator_engine.cpp
    thumb/thumbnailloadthread.cpp
    thumb/thumbnailloadthread_p.cpp
    thumb/thumbnailtask.cpp
    thumb/thumbnailsize.cpp

    fileio/loadsavethread.cpp
    fileio/loadingdescription.cpp
    fileio/loadingcache.cpp
    fileio/loadingcacheinterface.cpp
    fileio/loadsavetask.cpp
)

if(ENABLE_MEDIAPLAYER)

    set(libthreadimageio_SRCS
        ${libthreadimageio_SRCS}
        video/videostripfilter.cpp
        video/videodecoder.cpp
        video/videodecoder_p.cpp
        video/videothumbwriter.cpp
        video/videothumbnailer.cpp
    )

endif()

include_directories(
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamcore
add_library(core_threadimageio_obj OBJECT ${libthreadimageio_SRCS})

target_compile_definitions(core_threadimageio_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
