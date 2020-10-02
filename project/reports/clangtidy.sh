#!/bin/bash

# Copyright (c) 2013-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Clang tidy lint static analyzer on whole digiKam source code.
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

. ./common.sh

checksCPUCores

/usr/share/clang/run-clang-tidy.py -quiet -j$CPU_CORES -p  ../../build/ | tee clang-tidy.log

