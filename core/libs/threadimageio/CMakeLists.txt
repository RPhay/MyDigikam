#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libthreadimageio_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/engine/dfileoperations.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/engine/filereadwritelock.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/engine/managedloadsavethread.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/engine/sharedloadsavethread.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/preview/previewloadthread.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/preview/previewtask.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/preview/previewsettings.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailbasic.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailcreator.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailcreator_freedesktop.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailcreator_database.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailcreator_engine.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailloadthread.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailloadthread_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailtask.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailsize.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/fileio/loadsavethread.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/fileio/loadingdescription.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/fileio/loadingcache.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/fileio/loadingcacheinterface.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/fileio/loadsavetask.cpp
)

if(ENABLE_MEDIAPLAYER)

    set(libthreadimageio_SRCS
        ${libthreadimageio_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/video/videostripfilter.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/video/videodecoder.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/video/videodecoder_p.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/video/videothumbwriter.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/video/videothumbnailer.cpp
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

add_library(threadimageio_src OBJECT ${libthreadimageio_SRCS})
