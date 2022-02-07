#!/bin/bash

# Copyright (c) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Script to run all Continuous Integration scripts at the same time.

./cppcheck.sh && ./krazy.sh && ./clangscan.sh && ./clazy.sh && ./coverity.sh
