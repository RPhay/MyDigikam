#!/bin/bash

# Rules to prepare Mageia Linux host.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

#################################################################################################

echo -e "---------- Update Linux Mageia Host\n"

urpmi --auto --auto-update

#################################################################################################

if [[ "$(arch)" = "x86_64" ]] ; then
    LIBSUFFIX=lib64
else
    LIBSUFFIX=lib
fi

echo -e "---------- Install New Development Packages\n"

# Packages for base dependencies and Qt5.
urpmi --auto \
      wget \
      tar \
      bzip2 \
      gettext \
      git \
      subversion \
      libtool \
      which \
      fuse \
      automake \
      cmake \
      ccache \
      gcc-c++ \
      yasm \
      patch \
      libdrm-devel \
      libxcb \
      libxcb-devel \
      xcb-util-keysyms-devel \
      xcb-util-devel \
      xkeyboard-config \
      xscreensaver \
      gperf \
      ruby \
      bison \
      flex \
      zlib-devel \
      expat-devel \
      fuse-devel \
      glibc-devel \
      mysql-devel \
      eigen3-devel \
      cppunit-devel \
      libstdc++-devel \
      libstdc++-static-devel \
      libxml2-devel \
      lcms2-devel \
      glibc-devel \
      libudev-devel \
      sqlite-devel \
      libexif-devel \
      libxslt-devel \
      xz-devel \
      lz4-devel \
      inotify-tools-devel \
      cups-devel \
      openal-soft-devel \
      lib64pulseaudio-devel \
      libical-devel \
      libcap-devel \
      fontconfig-devel \
      patchelf \
      dpkg \
      ninja \
      clang \
      cdialog \
      python \
      ruby \
      ruby-devel \
      sqlite3-devel \
      boost-devel \
      gphoto2-devel \
      sane-backends \
      nodejs-devel \
      python3-lxml \
      perl-YAML-PP \
      perl-YAML-Syck \
      appstream \
      openssh-askpass \
      ${LIBSUFFIX}openssl-devel \
      ${LIBSUFFIX}nss-devel \
      ${LIBSUFFIX}xkbcommon-devel \
      ${LIBSUFFIX}xkbfile-devel \
      ${LIBSUFFIX}xinerama-devel \
      ${LIBSUFFIX}sane1-devel \
      ${LIBSUFFIX}xcb-util1 \
      ${LIBSUFFIX}xcb-util-cursor-devel \
      ${LIBSUFFIX}xcb-util-wm-devel \
      ${LIBSUFFIX}xcb-xrm-devel \
      ${LIBSUFFIX}xi-devel \
      ${LIBSUFFIX}xtst-devel \
      ${LIBSUFFIX}xrandr-devel \
      ${LIBSUFFIX}xcursor-devel \
      ${LIBSUFFIX}xcomposite-devel \
      ${LIBSUFFIX}xrender-devel \
      ${LIBSUFFIX}xscrnsaver-devel \
      ${LIBSUFFIX}ltdl-devel \
      ${LIBSUFFIX}glib2.0-devel \
      ${LIBSUFFIX}usb1.0-devel \
      ${LIBSUFFIX}jpeg-devel \
      ${LIBSUFFIX}jasper-devel \
      ${LIBSUFFIX}png-devel \
      ${LIBSUFFIX}tiff-devel \
      ${LIBSUFFIX}lqr-devel \
      ${LIBSUFFIX}fftw-devel \
      ${LIBSUFFIX}curl-devel \
      ${LIBSUFFIX}wayland-devel \
      ${LIBSUFFIX}clang-devel \
      ${LIBSUFFIX}sm-devel \
      ${LIBSUFFIX}freeglut-devel \
      ${LIBSUFFIX}input-devel \
      ${LIBSUFFIX}fdk-aac-devel \
      ${LIBSUFFIX}x264-devel \
      ${LIBSUFFIX}x265-devel \
      ${LIBSUFFIX}xvidcore-devel \
      ${LIBSUFFIX}vpx-devel \
      ${LIBSUFFIX}theora-devel \
      ${LIBSUFFIX}vorbis-devel \
      ${LIBSUFFIX}opencore-amr-devel \
      ${LIBSUFFIX}rtmp-devel \
      ${LIBSUFFIX}opus-devel \
      ${LIBSUFFIX}speex-devel \
      ${LIBSUFFIX}mp3lame-devel \
      ${LIBSUFFIX}freetype2-devel \
      ${LIBSUFFIX}ass-devel


if [[ "$DK_QTVERSION" = "5.14" ]] ; then

    urpmi --auto \
          ${LIBSUFFIX}mesagl1-devel \
          ${LIBSUFFIX}mesaglu1-devel \
          ${LIBSUFFIX}mesaegl1-devel \
          ${LIBSUFFIX}mesaegl1

fi

if [[ "$DK_QTVERSION" = "5.15" || "$DK_QTVERSION" = "5.15-LTS" ]] ; then

    urpmi --auto \
          ${LIBSUFFIX}xcb-util-cursor-devel \
          ${LIBSUFFIX}xcb-util-image-devel \
          ${LIBSUFFIX}xcb-util-renderutil-devel \
          ${LIBSUFFIX}xcb-util-wm-devel \
          ${LIBSUFFIX}xcb-xrm-devel

fi

if [[ $DK_QTVERSION == 6.* ]] ; then

    urpmi --auto \
          libxkbcommon-utils \
          ${LIBSUFFIX}mesaegl-devel

fi

echo -e "---------- Clean-up Old Packages\n"

# Remove system based devel package to prevent conflict with new one.
urpme --auto --force ${LIBSUFFIX}qt5core5 || true

# Clean up previous openssl install

rm -fr /usr/local/lib/libssl.a    || true
rm -fr /usr/local/lib/libcrypto.a || true
rm -fr /usr/local/include/openssl || true

# Workaround for: On Mageia 6, .pc files in /usr/lib/pkgconfig are not recognized
# However, this is where .pc files get installed when building libraries... (FIXME)
# I found this by comparing the output of librevenge's "make install" command
# between Ubuntu and CentOS 6
ln -sf /usr/share/pkgconfig /usr/lib/pkgconfig
