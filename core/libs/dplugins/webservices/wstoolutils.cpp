/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-12
 * Description : Web Service tool utils methods
 *
 * Copyright (C) 2014-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QRandomGenerator>
#include <QStandardPaths>
#include <QApplication>
#include <QByteArray>
#include <QBuffer>
#include <QTime>

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
    QRandomGenerator* const generator = QRandomGenerator::global();

    for (int i = 0 ; i < length ; ++i)
    {
        const int index = generator->bounded(possibleCharacters.length());
        QChar nextChar  = possibleCharacters.at(index);
        randomString.append(nextChar);
    }

    return randomString;
}

// ------------------------------------------------------------------------------------

QSettings* WSToolUtils::getOauthSettings(QObject* const parent)
{
    QString dkoauth = oauthConfigFile();

    return (new QSettings(dkoauth, QSettings::IniFormat, parent));
}

void WSToolUtils::saveToken(const QString& name, const QString& token)
{
    QString dkoauth = oauthConfigFile();

    QSettings settings(dkoauth, QSettings::IniFormat);
    QByteArray code = token.toLatin1().toBase64();

    settings.beginGroup(name);
    settings.setValue(QLatin1String("refreshToken"), code);
    settings.endGroup();
}

QString WSToolUtils::readToken(const QString& name)
{
    QString dkoauth = oauthConfigFile();

    QSettings settings(dkoauth, QSettings::IniFormat);

    settings.beginGroup(name);
    QByteArray code = settings.value(QLatin1String("refreshToken")).toByteArray();
    settings.endGroup();

    return QString::fromLatin1(QByteArray::fromBase64(code));
}

void WSToolUtils::clearToken(const QString& name)
{
    QString dkoauth = oauthConfigFile();

    QSettings settings(dkoauth, QSettings::IniFormat);

    settings.beginGroup(name);
    settings.remove(QString());
    settings.endGroup();
}

QString WSToolUtils::oauthConfigFile()
{
    QString dkoauth = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
                      QLatin1String("/digikam_oauthrc");

    return dkoauth;
}

} // namespace Digikam
