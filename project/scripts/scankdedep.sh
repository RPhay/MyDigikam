#!/bin/bash

# Copyright (c) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Script to show all KF5 dependencies by macro includes scan

grep -r "#include <k" ../../core/*          | \
    grep -v "#include <klocalizedstring.h>" | \
    grep -v "config"                        | \
    grep -v "kernel"                        | \
    grep -v "kvm"                           | \
    grep -v "kstat"                         | \
    grep -v "keychain"                      | \
    grep -v "scankdedep.sh"
