#!/bin/bash

# SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

ORIG_WD="`pwd`"
API_DIR="${ORIG_WD}/../../html"
WEBSITE_DIR="${ORIG_WD}/site"

rm -fr $API_DIR || true
rm -fr $WEBSITE_DIR || true

cd ${ORIG_WD}/../../ && doxygen
cd ${ORIG_WD}

git clone git@invent.kde.org:websites/digikam-org.git $WEBSITE_DIR

cd $WEBSITE_DIR

git checkout -b dev remotes/origin/dev

cd $WEBSITE_DIR
rm -r $WEBSITE_DIR/static/api || true
mkdir -p $WEBSITE_DIR/static/api
cd $API_DIR
cp -r ./ $WEBSITE_DIR/static/api/
cd $WEBSITE_DIR

# Add new report contents in dev branch

git add $WEBSITE_DIR/static/api
git commit . -m"update API documentation"
git push

# update master branch

git checkout master
git merge dev -m"Update API documentation"
git push

echo "API documentation published to https://www.digikam.org/api"
echo "Web site will be synchronized in few minutes..."
