#!/bin/bash

# Copyright (c) 2008-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
#
#
# Copy this script on root folder where are source code

./configure --prefix=/usr                \
            --extra-libs=-lstdc++        \
            --disable-doc                \
            --disable-debug              \
            --disable-programs           \
            --disable-outdevs            \
            --enable-shared              \
            --enable-gpl                 \
            --enable-version3            \
            --enable-nonfree             \
            --enable-runtime-cpudetect   \
            --enable-libopencore-amrnb   \
            --enable-libopencore-amrwb   \
            --enable-libfdk-aac          \
            --enable-libmp3lame          \
            --enable-libspeex            \
            --enable-libtheora           \
            --enable-libvorbis           \
            --enable-libx264             \
            --enable-libx265             \
            --enable-libaom              \
            --enable-libxvid             \
            --enable-libvpx              \
            --enable-libopus             \
            --enable-librtmp             \
            --enable-libfreetype         \
            --enable-libass              \
            --enable-opengl              \
