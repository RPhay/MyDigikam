#!/bin/bash

# Script to cleanup installation of KF5 framework from the host system.
# This script must be run as sudo
#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
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

rm -vfr /usr/lib64/libKF5*
rm -vfr /usr/share/ECM
rm -vfr /usr/include/KF5
rm -vfr /usr/lib64/cmake/KF5*
rm -vfr /usr/include/astro
rm -vfr /usr/include/marble
rm -vfr /usr/share/marble
rm -vfr /usr/lib64/marble
rm -vfr /usr/lib64/libastro*
rm -vfr /usr/lib64/libmarble*
rm -vfr /usr/lib64/cmake/Marble
rm -vfr /usr/lib64/cmake/Astro
