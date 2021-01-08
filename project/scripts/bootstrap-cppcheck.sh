#!/bin/bash

# Copyright (c) 2008-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Copy this script on root folder where are source code

#export VERBOSE=1

# We will work on command line using MinGW compiler
export MAKEFILES_TYPE='Unix Makefiles'

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

export Options='-DANALYZE_MEMORY=OFF \
                -DANALYZE_ADDRESS=OFF \
                -DANALYZE_THREAD=OFF \
                -DANALYZE_UNDEFINED=OFF \
                -DANALYZE_DATAFLOW=OFF \
                -DWARNINGS_ARE_ERRORS=OFF \
                -DENABLE_CHECK_INTERNAL=OFF \
                -DENABLE_OSS_FUZZ=ON \
                -DBUILD_GUI=OFF \
                -DWITH_QCHART=OFF \
                -DHAVE_RULES=OFF \
                -DUSE_Z3=OFF \
                -DBUILD_TESTS=OFF \
                '

cmake -G "$MAKEFILES_TYPE" . \
      -DCMAKE_INSTALL_PREFIX=/usr \
      -Wno-dev \
      $Options \
      ..

