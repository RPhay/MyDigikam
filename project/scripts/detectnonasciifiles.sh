#!/bin/bash

# Copyright (c) 2008-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Copy this script on root folder where are source code

for f in `find \( -name '*.cpp' -o -name '*.h' \)` ; do

    if test -f $f ; then

        CHARSET="$( file -bi "$f"|awk -F "=" '{print $2}')"

        if [ "$CHARSET" != us-ascii ]; then

            echo -e "$f => $CHARSET"

        fi

    else

        echo -e "\nSkipping $f - it's a regular file";

    fi

done
