#!/bin/bash

# Script to cleanup installation of Qt framework from the host system.
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

rm -vfr /usr/include/Qt*
rm -vfr /usr/lib/libQt*
rm -vfr /usr/lib/cmake/Qt*
rm -vfr /usr/libexec/Qt*
rm -vfr /usr/qml
rm -vfr /usr/resources/qt*
rm -vfr /usr/translations
rm -vfr /usr/plugins
rm -vfr /usr/doc
rm -vfr /usr/mkspecs/q*.pri
