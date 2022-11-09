#!/bin/bash

# Ask to run as root
(( EUID != 0 )) && exec sudo -- "$0" "$@"

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

ORIG_WD="`pwd`"

cd $ORIG_WD/7.x/project/bundles/macports
./update.sh

cd $ORIG_WD/8.x/project/bundles/macports
./update.sh

cd $ORIG_WD

