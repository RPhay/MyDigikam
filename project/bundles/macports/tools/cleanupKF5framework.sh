#!/bin/bash

# Script to cleanup installation of KF5 framework from the Macports install.
# This script must be run as sudo
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Ask to run as root
(( EUID != 0 )) && exec sudo -- "$0" "$@"

# halt on error
set -e

. ../config.sh
. ../common.sh

#################################################################################################

# Paths rules
ORIG_PATH="$PATH"
export PATH=$INSTALL_PREFIX/bin:/$INSTALL_PREFIX/sbin:$ORIG_PATH

CommonChecks

echo $INSTALL_PREFIX

rm -vfr $INSTALL_PREFIX/lib/libKF5*
rm -vfr $INSTALL_PREFIX/share/ECM
rm -vfr $INSTALL_PREFIX/include/KF5
rm -vfr $INSTALL_PREFIX/lib/cmake/KF5*
rm -vfr $INSTALL_PREFIX/include/astro
rm -vfr $INSTALL_PREFIX/include/marble
rm -vfr $INSTALL_PREFIX/data
rm -vfr $INSTALL_PREFIX/lib/marble
rm -vfr $INSTALL_PREFIX/lib/libastro*
rm -vfr $INSTALL_PREFIX/lib/libmarble*
#rm -vfr $INSTALL_PREFIX/lib/cmake/Marble
#rm -vfr $INSTALL_PREFIX/lib/cmake/Astro
