/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-08
 * Description : global macros, variables and flags - Private header.
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_GLOBALS_P_H
#define DIGIKAM_GLOBALS_P_H

#include "digikam_globals.h"

// Qt includes

#include <QObject>
#include <QDir>
#include <QList>
#include <QImageReader>
#include <QImageWriter>
#include <QByteArray>
#include <QShortcut>
#include <QApplication>
#include <QStandardPaths>
#include <QTranslator>
#include <QLibrary>
#include <QLibraryInfo>
#include <QSettings>
#include <QSysInfo>
#include <QMimeType>
#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "drawdecoder.h"
#include "rawcameradlg.h"

// Windows includes

#ifdef HAVE_DRMINGW
#   include <windows.h>
#endif

#endif // DIGIKAM_GLOBALS_P_H
