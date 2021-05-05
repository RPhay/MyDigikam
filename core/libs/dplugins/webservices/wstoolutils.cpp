/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-12
 * Description : Web Service tool utils methods
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "wstoolutils.h"

// Qt includes

#include <QStandardPaths>
#include <QApplication>
#include <QByteArray>
#include <QBuffer>
#include <QTime>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
#   include <QRandomGenerator>
#endif

namespace Digikam
{

QDir WSToolUtils::makeTemporaryDir(const char* prefix)
{
    QString subDir = QString::fromLatin1("digikam-%1-%2").arg(QString::fromUtf8(prefix)).arg(qApp->applicationPid());
    QString path   = QDir(QDir::tempPath()).filePath(subDir);

    if (!QDir().exists(path))
    {
        QDir().mkpath(path);
    }

    return QDir(path);
}

// ------------------------------------------------------------------------------------

void WSToolUtils::removeTemporaryDir(const char* prefix)
{
    QString subDir = QString::fromLatin1("digikam-%1-%2").arg(QString::fromUtf8(prefix)).arg(qApp->applicationPid());
    QString path   = QDir(QDir::tempPath()).filePath(subDir);

    if (QDir().exists(path))
    {
        QDir(path).removeRecursively();
    }
}

// ------------------------------------------------------------------------------------

QString WSToolUtils::randomString(const int& length)
{
    const QString possibleCharacters(QLatin1String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));

    QString randomString;

#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))

    qsrand((uint)QTime::currentTime().msec());

#else

    QRandomGenerator* const generator = QRandomGenerator::global();

#endif

    for (int i = 0 ; i < length ; ++i)
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))

        const int index = qrand() % possibleCharacters.length();

#else

        const int index = generator->bounded(possibleCharacters.length());

#endif

        QChar nextChar  = possibleCharacters.at(index);
        randomString.append(nextChar);
    }

    return randomString;
}

// ------------------------------------------------------------------------------------

QSettings* WSToolUtils::getOauthSettings(QObject* const parent)
{
    QString dkoauth = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
                      QLatin1String("/digikam_oauthrc");

    return (new QSettings(dkoauth, QSettings::IniFormat, parent));
}

} // namespace Digikam
