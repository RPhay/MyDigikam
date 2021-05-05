#!/bin/bash

# Copyright (c) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Script to sign bundle files with a GPG key
# GPG key password must be present as simple text file in ~/.gnupg/dkorg-gpg-pwd.txt

ALLFILES=$(find . -type f -maxdepth 1)

for BUNDLE in $ALLFILES ; do

    BASENAME=$(basename $BUNDLE)

    if [[ $BASENAME != $(basename $BASH_SOURCE) ]] && [[ $BASENAME != ".." ]] && [[ $BASENAME != "." ]] ; then

        cat ~/.gnupg/dkorg-gpg-pwd.txt | gpg --batch --yes --passphrase-fd 0 -stabv "$BUNDLE" && mv "$BUNDLE.asc" "$BUNDLE.sig"
    fi

done
