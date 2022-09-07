#!/bin/sh

# Copyright (c) 2013-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
# This script must be run as sudo
#
# Redistribution and use is allowed according to the terms of the BSD license.
#

# Pre-processing checks

# Ask to run as root
(( EUID != 0 )) && exec sudo -- "$0" "$@"

# halt on error
set -e

. ../common.sh
StartScript
CommonChecks

# Update Macports binary

port selfupdate

# Update all already install packages

port upgrade outdated

TerminateScript
