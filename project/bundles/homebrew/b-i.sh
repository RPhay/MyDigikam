#!/bin/bash

# This script runs steps 03 and 04 together to create a new install package

(( EUID != 0 )) && exec sudo -- "$0" "$@"

ORIG_WD="`pwd`"
. ./03-build-digikam.sh
cd $ORIG_WD
. ./04mm-build-installer.sh
