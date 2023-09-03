#!/bin/bash

# Script to patch Qt6 minimal version in KF6 components.
#
# SPDX-FileCopyrightText: 2015-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

cd "$1"
find . -type f \( -name 'CMakeLists.txt' \) -print0 | xargs -0 sed -i 's/REQUIRED_QT_VERSION 6.5.0/REQUIRED_QT_VERSION 6.4.1/g'
