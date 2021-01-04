#!/bin/bash

# Script to update digiKam installer.
#
# Copyright (c) 2013-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

. ./config.sh
. ./common.sh
StartScript

echo "++++++++++++++++   Build 64 bits Installer   ++++++++++++++++++++++++++++++++++"
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

./03-build-digikam.sh

sed -e "s/DK_DEBUG=1/DK_DEBUG=0/g"           ./config.sh > ./tmp.sh ; mv -f ./tmp.sh ./config.sh

./04-build-installer.sh

sed -e "s/DK_DEBUG=0/DK_DEBUG=1/g"           ./config.sh > ./tmp.sh ; mv -f ./tmp.sh ./config.sh

./04-build-installer.sh

sed -e "s/DK_DEBUG=1/DK_DEBUG=0/g"           ./config.sh > ./tmp.sh ; mv -f ./tmp.sh ./config.sh

echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

TerminateScript
