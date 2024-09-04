#!/bin/bash

# SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

while true;
do
    DATE=`date +%H%M`

    if [[ $DATE == "0300" || $DATE == "0900" || $DATE == "1500" || $DATE == "2100"  ]] ; then

        echo "Windows bundle started at $(date)"
        git pull
        ./update.sh
        echo "Windows bundle completed at $(date)"

    fi

    sleep 20s

done
