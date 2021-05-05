#!/bin/bash

while true;
do
    DATE=`date +%H%M`

    if [[ $DATE == "2300" ]] ; then

        echo "macOS bundle started at $(date)"
        cd /Users/gilles/Documents/7.x/project/bundles/macports
        git pull
        ./update.sh
        echo "macOS bundle completed at $(date)"

    fi

    sleep 20s

done
