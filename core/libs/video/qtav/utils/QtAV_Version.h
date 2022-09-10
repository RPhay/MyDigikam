/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_VERSION_H
#define QTAV_VERSION_H

#define QTAV_MAJOR 1
#define QTAV_MINOR 14
#define QTAV_PATCH 0

#define QTAV_VERSION_MAJOR(V) ((V & 0xff0000) >> 16)
#define QTAV_VERSION_MINOR(V) ((V & 0xff00) >> 8)
#define QTAV_VERSION_PATCH(V) (V & 0xff)

#define QTAV_VERSION_CHK(major, minor, patch) \
    (((major & 0xff) << 16) | ((minor & 0xff) << 8) | (patch & 0xff))

#define QTAV_VERSION QTAV_VERSION_CHK(QTAV_MAJOR, QTAV_MINOR, QTAV_PATCH)

/*!
 *Stringify \a x.
 */
#define _TOSTR(x)        #x

/*!
 Stringify \a x, perform macro expansion.
 */
#define TOSTR(x)         _TOSTR(x)

// the following are compile time version
// C++11 requires a space between literal and identifier

#define QTAV_VERSION_STR TOSTR(QTAV_MAJOR) "." TOSTR(QTAV_MINOR) "." TOSTR(QTAV_PATCH)

#endif // QTAV_VERSION_H
