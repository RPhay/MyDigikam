/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-12-20
 * Description : Interface for version file naming
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#include "versionnamingscheme.h"

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

QVariant DefaultVersionNamingScheme::initialCounter()
{
    // start with _v1

    return 1;
}

QVariant DefaultVersionNamingScheme::incrementedCounter(const QVariant& counter)
{
    return counter.toInt() + 1;
}

QString DefaultVersionNamingScheme::baseName(const QString& currentPath, const QString& fileName,
                                             QVariant* counter, QVariant* intermediateCounter)
{
    Q_UNUSED(currentPath);

    // Perl: /^(.+?)(_v\d+?)?(-\d+?)?\.([^\.]+?)$/
    // But setMinimal() cannot replace Perl's non-greedy quantifiers, so we need three regexps

    int index                = fileName.lastIndexOf(QLatin1Char('.'));
    QString completeBaseName = (index == -1) ? fileName : fileName.left(index);

    // DSC000636_v5-3.JPG: intermediate

    QRegExp versionIntermediate(QLatin1String("(.+)_v(\\d+)-(\\d+)"));

    if (versionIntermediate.exactMatch(completeBaseName))
    {
        if (counter)
        {
            *counter = versionIntermediate.cap(2).toInt();
        }

        if (intermediateCounter)
        {
            *intermediateCounter = versionIntermediate.cap(3).toInt();
        }

        return versionIntermediate.cap(1);
    }

    // DSC000636_v5.JPG: version

    QRegExp version(QLatin1String("(.+)_v(\\d+)"));

    if (version.exactMatch(completeBaseName))
    {
        if (counter)
        {
            *counter = version.cap(2).toInt();
        }

        return version.cap(1);
    }

    // DSC000636.JPG: original file or different naming scheme

    return completeBaseName;
}

QString DefaultVersionNamingScheme::versionFileName(const QString& currentPath,
                                                    const QString& baseName, const QVariant& counter)
{
    Q_UNUSED(currentPath);

    return QString::fromUtf8("%1_v%2").arg(baseName).arg(counter.toInt());
}

QString DefaultVersionNamingScheme::intermediateFileName(const QString& currentPath, const QString& baseName,
                                                         const QVariant& version, const QVariant& counter)
{
    Q_UNUSED(currentPath);

    return QString::fromUtf8("%1_v%2-%3").arg(baseName).arg(version.toInt()).arg(counter.toInt());
}

QString DefaultVersionNamingScheme::directory(const QString& currentPath, const QString& fileName)
{
    Q_UNUSED(fileName);

    return currentPath;
}

QString DefaultVersionNamingScheme::intermediateDirectory(const QString& currentPath, const QString& fileName)
{
    Q_UNUSED(fileName);

    return currentPath;
}

} // namespace Digikam
