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

#ifndef QTAV_INTERNAL_H
#define QTAV_INTERNAL_H

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QObject>
#include "QtAV/private/AVCompat.h"

namespace QtAV {
namespace Internal {
namespace Path {
QString toLocal(const QString& fullPath);
/// It may be variant from Qt version
/*!
 * \brief appDataDir
 * use QStandardPath::AppDataLocation for Qt>=5.4, or QStarnardPath::DataLocation for Qt<5.4
 * \return
 */
QString appDataDir();
// writable font dir. it's fontsDir() if writable or appFontsDir()/fonts
/*!
 * \brief appFontsDir
 * It's "appDataDir()/fonts". writable fonts dir from Qt may be not writable (OSX)
 * TODO: It's a writable fonts dir for QtAV apps. Equals to fontsDir() if it's writable, for example "~/.fonts" for linux and "<APPROOT>/Documents/.fonts" for iOS.
 *       If fontsDir() is not writable, it's equals to "appDataDir()/fonts".
 */
QString appFontsDir();
// usually not writable. Maybe empty for some platforms, for example winrt
QString fontsDir();
}

// TODO: use namespace Options
QString optionsToString(void* obj);
void setOptionsToFFmpegObj(const QVariant& opt, void* obj);
void setOptionsToDict(const QVariant& opt, AVDictionary** dict);
// set qobject meta properties
void setOptionsForQObject(const QVariant& opt, QObject* obj);

} //namespace Internal
} //namespace QtAV
#endif //QTAV_INTERNAL_H
