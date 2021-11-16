#!/bin/bash

# Copyright (c) 2008-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

echo "Start M6 VM..."
VBoxManage startvm M6

echo "Start M7 VM..."
VBoxManage startvm M7

echo "Start MX VM..."
VBoxManage startvm MX

echo "All VM started."

exit 0
