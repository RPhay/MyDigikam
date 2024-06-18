#!/bin/bash

# Script to cleanup installation of KDE framework from the host system.
# This script must be run as sudo
#
# SPDX-FileCopyrightText: 2015-2024 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

#################################################################################################
# Pre-processing checks

. ../common.sh
ChecksRunAsRoot

rm -vfr /usr/lib64/libKF*
rm -vfr /usr/share/ECM
rm -vfr /usr/include/KF*
rm -vfr /usr/lib/x86_64-linux-gnu/cmake/KF*
rm -vfr /usr/include/astro
rm -vfr /usr/include/marble
rm -vfr /usr/share/marble
rm -vfr /usr/share/k*5
rm -vfr /usr/plugins/k*5
rm -vfr /usr/share/k*6
rm -vfr /usr/plugins/k*6
rm -vfr /usr/plugins/imageformats/kimg*
rm -vfr /usr/plugins/kauth
rm -vfr /usr/plugins/plasma
rm -vfr /usr/plugins/styles/breeze*
rm -vfr /usr/plugins/iconengines/KIcon*
rm -vfr /usr/plugins/designer/k*
rm -vfr /usr/plugins/org.kde*
rm -vfr /usr/plugins/marble*
rm -vfr /usr/plugins/kcm*
rm -vfr /usr/lib64/marble
rm -vfr /usr/lib64/libastro*
rm -vfr /usr/lib64/libmarble*
rm -vfr /usr/lib64/cmake/Marble
rm -vfr /usr/lib64/cmake/Astro
