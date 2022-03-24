# Script to build ffmpeg libraries for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# NOTE: see build rules and tips at:
#       https://github.com/pyke369/sffmpeg/blob/master/CMakeLists.txt
#       https://www.linuxfromscratch.org/blfs/view/svn/multimedia/ffmpeg.html
#       https://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu
#

SET(PREFIX_ext_ffmpeg "${EXTPREFIX}")

ExternalProject_Add(ext_ffmpeg
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL https://ffmpeg.org/releases/ffmpeg-4.4.1.tar.xz
    URL_MD5 41a1ca8693acc56088e8eaf3899b91ea

    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREFIX_ext_ffmpeg}
                                             --extra-libs=-lstdc++
                                             --disable-doc
                                             --disable-debug
                                             --disable-programs
                                             --disable-outdevs
                                             --enable-shared
                                             --enable-gpl
                                             --enable-version3
                                             --enable-nonfree
                                             --enable-runtime-cpudetect
                                             --enable-libopencore-amrnb
                                             --enable-libopencore-amrwb
                                             --enable-libfdk-aac
                                             --enable-libmp3lame
                                             --enable-libspeex
                                             --enable-libtheora
                                             --enable-libvorbis
                                             --enable-libx264
                                             --enable-libx265
                                             --enable-libaom
                                             --enable-libxvid
                                             --enable-libvpx
                                             --enable-libopus
                                             --enable-librtmp
                                             --enable-libfreetype
                                             --enable-libass
                                             --enable-opengl

    UPDATE_COMMAND ""
    ALWAYS 0
)
