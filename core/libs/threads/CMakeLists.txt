#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(${CMAKE_SOURCE_DIR}/core/libs/threads/libdthread_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/threads/actionthreadbase.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threads/threadmanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threads/workerobject.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threads/dynamicthread.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/threads/parallelworkers.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(dthread_src OBJECT ${libdthread_SRCS})
