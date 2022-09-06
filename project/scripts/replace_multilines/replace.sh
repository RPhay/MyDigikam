#!/bin/bash

#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find .. -type f -name "*.cpp" \
    -exec ./replace.py \
      -F ./input.txt \
      -T ./output.txt \
      -i {} \;

find .. -type f -name "*.h" \
    -exec ./replace.py \
      -F ./input.txt \
      -T ./output.txt \
      -i {} \;

find .. -type f -name "*.cpp" \
    -exec ./replace.py \
      -F ./input_2.txt \
      -T ./output.txt \
      -i {} \;

find .. -type f -name "*.h" \
    -exec ./replace.py \
      -F ./input_2.txt \
      -T ./output.txt \
      -i {} \;
