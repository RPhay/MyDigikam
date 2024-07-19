/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "MarbleDirs.h"

// Qt includes

#include <QFile>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"

namespace Marble
{

QString MarbleDirs::path(const QString& relativePath)
{
    QString localpath  = localPath()       +
                         QDir::separator() +
                         relativePath;   // local path
    QString systempath = systemPath()      +
                         QDir::separator() +
                         relativePath;   // system path
    QString fullpath   = systempath;

    if (QFile::exists(localpath))
    {
        fullpath = localpath;
    }

    return QDir(fullpath).canonicalPath();
}

QString MarbleDirs::pluginPath(const QString& relativePath)
{
    const QString localpath  = pluginLocalPath()  +
                               QDir::separator()  +
                               relativePath; // local path
    const QString systempath = pluginSystemPath() +
                               QDir::separator()  +
                               relativePath; // system path
    QString fullpath         = systempath;

    if (QFile::exists(localpath))
    {
        fullpath = localpath;
    }

    return QDir(fullpath).canonicalPath();
}

QStringList MarbleDirs::entryList(const QString& relativePath, QDir::Filters filters)
{
    QStringList filesLocal  = QDir(MarbleDirs::localPath()  +
                                   QDir::separator()             +
                                   relativePath).entryList(filters);
    QStringList filesSystem = QDir(MarbleDirs::systemPath() +
                                   QDir::separator()             +
                                   relativePath).entryList(filters);
    QStringList allFiles(filesLocal);
    allFiles << filesSystem;

    // remove duplicate entries

    allFiles.sort();

    for (int i = 1 ; i < allFiles.size() ; ++i)
    {
        if (allFiles.at(i) == allFiles.at(i - 1))
        {
            allFiles.removeAt(i);
            --i;
        }
    }

    return allFiles;
}

QStringList MarbleDirs::pluginEntryList(const QString& relativePath, QDir::Filters filters)
{
    QStringList allFiles        = QDir(MarbleDirs::pluginLocalPath() +
                                       QDir::separator()                  +
                                       relativePath).entryList(filters);
    auto const pluginSystemPath = MarbleDirs::pluginSystemPath();

    if (!pluginSystemPath.isEmpty())
    {
        allFiles << QDir(pluginSystemPath + QDir::separator() + relativePath).entryList(filters);
    }

    // remove duplicate entries

    allFiles.sort();

    for (int i = 1 ; i < allFiles.size() ; ++i)
    {
        if (allFiles.at(i) == allFiles.at(i - 1))
        {
            allFiles.removeAt(i);
            --i;
        }
    }

    return allFiles;
}

QString MarbleDirs::systemPath()
{
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                          QLatin1String("digikam/marble"),
                                          QStandardPaths::LocateDirectory);

    return path;
}

QString MarbleDirs::localPath()
{
    return (
               QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
               QLatin1String("/digikam/marble_data")
           );
}

QString MarbleDirs::pluginLocalPath()
{
    return (
               QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
               QLatin1String("/digikam/marble_plugins")
           );
}

QString MarbleDirs::pluginSystemPath()
{

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    return (
               QLibraryInfo::path(QLibraryInfo::PluginsPath) +
               QLatin1String("/digikam/marble")
           );

#else

    return (
               QLibraryInfo::location(QLibraryInfo::PluginsPath) +
               QLatin1String("/digikam/marble")
           );

#endif

}

void MarbleDirs::debug()
{
    qCDebug(DIGIKAM_MARBLE_LOG) << "=== MarbleDirs: ===";
    qCDebug(DIGIKAM_MARBLE_LOG) << "Local Path        :" << localPath();
    qCDebug(DIGIKAM_MARBLE_LOG) << "Plugin Local Path :" << pluginLocalPath();
    qCDebug(DIGIKAM_MARBLE_LOG) << "System Path       :" << systemPath();
    qCDebug(DIGIKAM_MARBLE_LOG) << "Plugin System Path:" << pluginSystemPath();
    qCDebug(DIGIKAM_MARBLE_LOG) << "===================";
}

} // namespace Marble
