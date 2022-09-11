#!/bin/sh

# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

export MYDK="`pwd`\\bundle"
export KDEHOME=$MYDK
#export KDESYCOCA=$MYDK\\kbuildsycoca5
export XDG_DATA_DIRS=$MYDK\\share\\xdg\\
export KDEDIRS=$MYDK
export KDEDIR=$MYDK
export WINEPATH=$MYDK\\:$WINEPATH

cd ../bundle

wine64 kbuildsycoca5
wine64 showfoto

