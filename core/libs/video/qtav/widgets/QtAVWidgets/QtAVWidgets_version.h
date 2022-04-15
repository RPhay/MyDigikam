/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAVWIDGETS_VERSION_H
#define QTAVWIDGETS_VERSION_H

#include <QtAV/QtAV_version.h>

#define QTAVWIDGETS_MAJOR QTAV_MAJOR    //((QTAV_VERSION&0xff0000)>>16)
#define QTAVWIDGETS_MINOR QTAV_MAJOR    //((QTAV_VERSION&0xff00)>>8)
#define QTAVWIDGETS_PATCH QTAV_MAJOR    //(QTAV_VERSION&0xff)


#define QTAVWIDGETS_VERSION_MAJOR(V) ((V & 0xff0000) >> 16)
#define QTAVWIDGETS_VERSION_MINOR(V) ((V & 0xff00) >> 8)
#define QTAVWIDGETS_VERSION_PATCH(V) (V & 0xff)

#define QTAVWIDGETS_VERSION QTAV_VERSION_CHK(QTAVWIDGETS_MAJOR, QTAVWIDGETS_MINOR, QTAVWIDGETS_PATCH)

/* the following are compile time version */
/* C++11 requires a space between literal and identifier */
#define QTAVWIDGETS_VERSION_STR        TOSTR(QTAVWIDGETS_MAJOR) "." TOSTR(QTAVWIDGETS_MINOR) "." TOSTR(QTAVWIDGETS_PATCH)

#endif // QTAVWIDGETS_VERSION_H
