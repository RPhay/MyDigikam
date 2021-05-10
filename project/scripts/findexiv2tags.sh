#!/bin/bash

# ===========================================================
#
# This file is a part of digiKam project
# https://www.digikam.org
#
# Date:        2016-08-15
# Description: Script to find Exiv2 tags strings.
#
# Copyright (C) 2016-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

find ../../ -name "*.cpp" -type f | xargs grep -n "\"Exif\." | awk -F'[\"\"]' '{print $2}' |sort -u |cut -d' ' -f 1 | grep "Exif."
find ../../ -name "*.cpp" -type f | xargs grep -n "\"Iptc\." | awk -F'[\"\"]' '{print $2}' |sort -u |cut -d'=' -f 1 | grep "Iptc."
find ../../ -name "*.cpp" -type f | xargs grep -n "\"Xmp\." | awk -F'[\"\"]' '{print $2}' |sort -u |cut -d'=' -f 1 | grep "Xmp."
