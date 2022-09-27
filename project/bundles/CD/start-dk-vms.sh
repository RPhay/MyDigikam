#!/bin/bash

# SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

echo "Start M6 VM..."
VBoxManage startvm M6

echo "Start M7 VM..."
VBoxManage startvm M7

echo "Start MX VM..."
VBoxManage startvm MX

echo "All VM started."

exit 0
