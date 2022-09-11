#!/bin/bash

# SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

########################################################################
# Fix QtWebkit includes under MXE where camel case header files sound like not generated at install.

find . -regex '.*\.\(cpp\|h\)' -type f -print0 | xargs -r0 sed -e 's/#include <QWebView>/#include <qwebview.h>/g' -i
find . -regex '.*\.\(cpp\|h\)' -type f -print0 | xargs -r0 sed -e 's/#include <QWebPage>/#include <qwebpage.h>/g' -i
find . -regex '.*\.\(cpp\|h\)' -type f -print0 | xargs -r0 sed -e 's/#include <QWebFrame>/#include <qwebframe.h>/g' -i
find . -regex '.*\.\(cpp\|h\)' -type f -print0 | xargs -r0 sed -e 's/#include <QWebElement>/#include <qwebelement.h>/g' -i
find . -regex '.*\.\(cpp\|h\)' -type f -print0 | xargs -r0 sed -e 's/#include <QWebHistory>/#include <qwebhistory.h>/g' -i
