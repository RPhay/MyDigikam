#!/bin/sh

# Copyright (c) 2013-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
# This script must be run as sudo
#
# Redistribution and use is allowed according to the terms of the BSD license.
#

# Ask to run as root
(( EUID != 0 )) && exec sudo -- "$0" "$@"

# halt on error
set -e

# Pre-processing checks

. ../common.sh
StartScript
CommonChecks

# Uninstall Macports packages

port -fp uninstall installed

# Remove Macports dirs

rm -rf    \
    $INSTALL_PREFIX \
    /Applications/DarwinPorts \
    /Applications/MacPorts \
    /Library/LaunchDaemons/org.macports.* \
    /Library/Receipts/DarwinPorts*.pkg \
    /Library/Receipts/MacPorts*.pkg \
    /Library/StartupItems/DarwinPortsStartup \
    /Library/Tcl/darwinports1.0 \
    /Library/Tcl/macports1.0 \
    ~/.macports

TerminateScript
