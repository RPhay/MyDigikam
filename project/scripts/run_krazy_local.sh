#!/bin/bash

# SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Script to run single check on current directory

FILES=$(find * .)

echo "FILES=$FILES"

#krazy2 --check i18ncheckarg $FILES
#krazy2 --check doublequote_chars $FILES
#krazy2 --check normalize $FILES
#krazy2 --check includes $FILES
#krazy2 --check strings $FILES
#krazy2 --check syscalls $FILES
#krazy2 --check postfixop $FILES
#krazy2 --check explicit $FILES
#krazy2 --check cpp $FILES
#krazy2 --check typedefs $FILES
#krazy2 --check insecurenet $FILES

krazy2 --check spelling $FILES
