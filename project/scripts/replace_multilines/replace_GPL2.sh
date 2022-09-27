#!/bin/bash

# Script to replace GPL2+ header with SDPX-GPL2 licensing
#
# SPDX-FileCopyrightText: 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find .. -type f -name "*.cpp" \
    -exec ./replace.py \
      -F ./GPL2_1.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.h" \
    -exec ./replace.py \
      -F ./GPL2_1.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.cpp" \
    -exec ./replace.py \
      -F ./GPL2_2.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.h" \
    -exec ./replace.py \
      -F ./GPL2_2.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.cpp" \
    -exec ./replace.py \
      -F ./GPL2_3.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.h" \
    -exec ./replace.py \
      -F ./GPL2_3.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.cpp" \
    -exec ./replace.py \
      -F ./GPL2_4.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.h" \
    -exec ./replace.py \
      -F ./GPL2_4.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.cpp" \
    -exec ./replace.py \
      -F ./GPL2_5.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.h" \
    -exec ./replace.py \
      -F ./GPL2_5.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.cpp" \
    -exec ./replace.py \
      -F ./GPL2_6.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;

find .. -type f -name "*.h" \
    -exec ./replace.py \
      -F ./GPL2_6.txt \
      -T ./SPDX-GPL2.txt \
      -v \
      -i {} \;



