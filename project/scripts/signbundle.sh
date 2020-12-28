#!/bin/bash

# Copyright (c) 2008-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Script to sign bundle with a GPG key

cat ~/.gnupg/dkorg-gpg-pwd.txt | gpg --batch --yes --passphrase-fd 0 -stabv "$1" && mv "$1.asc" "$1.sig"
