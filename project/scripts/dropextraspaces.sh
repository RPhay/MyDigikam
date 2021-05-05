#!/bin/bash

# ===========================================================
#
# This file is a part of digiKam project
# https://www.digikam.org
#
# Date:        2011-11-02
# Description: Script to drop trailling white space at end line.
#
# Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# ============================================================

find -name '*.cpp' -print0 | xargs -r0 sed -e 's/[[:blank:]]\+$//' -i
find -name '*.h' -print0 | xargs -r0 sed -e 's/[[:blank:]]\+$//' -i
