#!/bin/bash

# ===========================================================
#
# This file is a part of digiKam project
# https://www.digikam.org
#
# Date:        2016-08-15
# Description: Script to find no really internationlized source code.
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

stringA="klocalizedstring.h"
stringB="18n"

# run loop for each file in the directory

for i in `find \( -name '*.cpp' -o -name '*.h' \)` ; do

   # check if file contains "string B"
   # if true then filename is not printed

   if [[ `egrep $stringB $i | wc -l` -eq 0 ]] ; then

      # check if file contains "string A"
      # if false then file name is not printed

      if [[ `egrep $stringA $i | wc -l` -gt 0 ]] ; then

         # file name is printed only if "string A" is present and "string B" is absent

         echo "$i\n"

      fi

   fi

done
