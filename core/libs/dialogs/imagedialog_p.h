/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-03-13
 * Description : Image files selection dialog - private declarations.
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMAGE_DIALOG_P_H
#define DIGIKAM_IMAGE_DIALOG_P_H

#include "imagedialog.h"

// Qt includes

#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>
#include <QApplication>
#include <QMimeDatabase>
#include <QStyle>
#include <QLocale>
#include <QPixmap>
#include <QPainter>
#include <QScopedPointer>
#include <QToolTip>
#include <QDir>
#include <QTimer>
#include <QWheelEvent>
#include <QKeyEvent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "drawdecoder.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "thumbnailloadthread.h"
#include "ditemtooltip.h"
#include "dmetadata.h"
#include "loadingdescription.h"
#include "thumbnailsize.h"
#include "dfiledialog.h"

#endif // DIGIKAM_IMAGE_DIALOG_P_H
