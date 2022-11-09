#!/bin/bash

# Ask to run as root

if [[ $EUID -ne 0 ]]; then
    echo "This script should be run as root using sudo command."
    exit 1
fi

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

ORIG_WD="`pwd`"

cd $ORIG_WD/7.x/project/bundles/appimage
./update.sh

cd $ORIG_WD/8.x/project/bundles/appimage
./update.sh

cd $ORIG_WD
