#!/bin/bash

# Copyright (c) 2008-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

echo "Save M6 VM..."
VBoxManage controlvm M6 savestate

echo "Save M7 VM..."
VBoxManage controlvm M7 savestate

echo "Save MX VM..."
VBoxManage controlvm MX savestate

echo "All VM saved."

exit 0
