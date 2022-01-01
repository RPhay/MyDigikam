#!/bin/bash

# Script to cleanup installation of KF5 framework from the host system.
# This script must be run as sudo
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

#################################################################################################
# Pre-processing checks

. ../common.sh
ChecksRunAsRoot

rm -fr /usr/lib64/libKF5*
rm -fr /usr/share/ECM
rm -fr /usr/include/KF5
rm -fr /usr/lib64/cmake/KF5*
rm -fr /usr/include/marble
rm -fr /usr/share/marble
rm -fr /usr/lib64/marble
rm -fr /usr/lib64/cmake/Marble
