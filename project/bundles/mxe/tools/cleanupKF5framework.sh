#!/bin/bash

# Script to cleanup installation of KF5 framework from the MXE install.
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

cd ..
. ./common.sh
. ./config.sh

echo $MXE_INSTALL_PREFIX

rm -vfr $MXE_INSTALL_PREFIX/lib/libKF5*
rm -vfr $MXE_INSTALL_PREFIX/share/ECM
rm -vfr $MXE_INSTALL_PREFIX/include/KF5
rm -vfr $MXE_INSTALL_PREFIX/lib/cmake/KF5*
rm -vfr $MXE_INSTALL_PREFIX/include/astro
rm -vfr $MXE_INSTALL_PREFIX/include/marble
rm -vfr $MXE_INSTALL_PREFIX/data
rm -vfr $MXE_INSTALL_PREFIX/lib/marble
rm -vfr $MXE_INSTALL_PREFIX/lib/libastro*
rm -vfr $MXE_INSTALL_PREFIX/lib/libmarble*
#rm -vfr $MXE_INSTALL_PREFIX/lib/cmake/Marble
#rm -vfr $MXE_INSTALL_PREFIX/lib/cmake/Astro
