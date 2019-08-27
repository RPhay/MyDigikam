#!/bin/bash

# Copyright (c) 2008-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Script to show all KF5 dependencies by macro includes scan

grep --exclude-dir PORT.KF5 -r "#include <k" * | \
   grep -v "#include <klocalizedstring.h>" | \
   grep -v "config"                        | \
   grep -v "ipi"                           | \
   grep -v "action"
