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

#ifndef QTAV_INTERNAL_H
#define QTAV_INTERNAL_H

// Qt includes

#include <QString>
#include <QVariant>
#include <QObject>

// Local includes

#include "AVCompat.h"

namespace QtAV
{

namespace Internal
{

namespace Path
{

QString toLocal(const QString& fullPath);

/// It may be variant from Qt version

// writable font dir. it's fontsDir() if writable or appFontsDir()/fonts

/*!
 * \brief appFontsDir
 * Writable fonts dir from Qt may be not writable (OSX)
 * TODO: It's a writable fonts dir for QtAV apps. Equals to fontsDir() if it's writable,
 * for example "~/.fonts" for linux and "<APPROOT>/Documents/.fonts" for iOS.
 */
QString appFontsDir();

/**
 * usually not writable. Maybe empty for some platforms, for example winrt
 */
QString fontsDir();

} // namespace Path

// TODO: use namespace Options

QString optionsToString(void* obj);
void setOptionsToFFmpegObj(const QVariant& opt, void* obj);
void setOptionsToDict(const QVariant& opt, AVDictionary** dict);

/**
 * set qobject meta properties
 */
void setOptionsForQObject(const QVariant& opt, QObject* const obj);

} // namespace Internal

} // namespace QtAV

#endif // QTAV_INTERNAL_H
