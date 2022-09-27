#!/bin/bash

# SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copy this script on root folder where are source code

#export VERBOSE=1

if [ ! -d "build.tmp" ]; then
    mkdir build.tmp
fi

# We will work on command line using MinGW compiler
export MAKEFILES_TYPE='Unix Makefiles'

cmake -H./ \
      -B./build.tmp \
      -G "$MAKEFILES_TYPE" \
      -DCMAKE_INSTALL_PREFIX=/usr \
      -Wno-dev \
      -DJAS_ENABLE_SHARED=ON \
      -DJAS_ENABLE_HIDDEN=ON \
      -DJAS_ENABLE_32BIT=OFF \
      -DJAS_ENABLE_LIBJPEG=ON \
      -DJAS_ENABLE_LIBHEIF=OFF \
      -DJAS_ENABLE_OPENGL=ON \
      -DJAS_ENABLE_DOC=OFF \
      -DJAS_ENABLE_LATEX=OFF \
      -DJAS_ENABLE_PROGRAMS=OFF \
      -DJAS_ENABLE_MULTITHREADING_SUPPORT=ON \
      -DJAS_PREFER_PTHREAD=ON \
      -DJAS_PREFER_PTHREAD_TSS=OFF \
      -DJAS_STRICT=OFF
