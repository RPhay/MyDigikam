#!/bin/bash

# SPDX-FileCopyrightText: 2008-2025 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copy this script on root folder where are source code
# https://github.com/KDE/clazy

export MAKEFILES_TYPE='Unix Makefiles'

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

export Options='-DCLAZY_AST_MATCHERS_CRASH_WORKAROUND=ON \
                -DLINK_CLAZY_TO_LLVM=ON \
                -DAPPIMAGE_HACK=OFF \
                -DCLAZY_MAN_PAGE=OFF'

/opt/qt6/bin/cmake -G "$MAKEFILES_TYPE" . \
      -DCMAKE_INSTALL_PREFIX=/opt/clazy \
      -Wno-dev \
      -DCMAKE_BUILD_TYPE=Release \
      $Options \
      ..

