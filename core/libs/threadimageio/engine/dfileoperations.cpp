/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-12-10
 * Description : misc file operation methods
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "dfileoperations.h"
#include "digikam_config.h"

// C ANSI includes

#include <sys/types.h>
#include <sys/stat.h>

#ifndef Q_CC_MSVC
#   include <utime.h>
#else
#   include <sys/utime.h>
#endif

// Qt includes

#include <QByteArray>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QMimeType>
#include <QMimeDatabase>
#include <QDesktopServices>
#include <QFileInfo>
#include <QDirIterator>
#include <qplatformdefs.h>

#ifdef HAVE_DBUS
#   include <QDBusInterface>
#   include <QDBusPendingCall>
#endif

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "progressmanager.h"
#include "metaenginesettings.h"

namespace Digikam
{

bool DFileOperations::localFileRename(const QString& source,
                                      const QString& orgPath,
                                      const QString& destPath,
                                      bool ignoreSettings)
{
    QString dest = destPath;

    // check that we're not replacing a symlink

    QFileInfo info(dest);

    if (info.isSymLink())
    {
        dest = info.symLinkTarget();

        qCDebug(DIGIKAM_GENERAL_LOG) << "Target filePath" << destPath
                                     << "is a symlink pointing to" << dest
                                     << ". Storing image there.";
    }

#ifndef Q_OS_WIN

    // Store old permissions:
    // Just get the current umask.

    mode_t curr_umask = umask(S_IREAD | S_IWRITE);

    // Restore the umask.

    umask(curr_umask);

    // For new files respect the umask setting.

    mode_t filePermissions = (S_IREAD | S_IWRITE | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP) & ~curr_umask;

    // For existing files, use the mode of the original file.

    QT_STATBUF stbuf;

    if (QT_STAT(dest.toUtf8().constData(), &stbuf) == 0)
    {
        filePermissions = stbuf.st_mode;
    }

#endif // Q_OS_WIN

    if (!ignoreSettings && !MetaEngineSettings::instance()->settings().updateFileTimeStamp)
    {
        copyModificationTime(source, orgPath);
    }

    // remove dest file if it exist

    if (orgPath != dest && QFile::exists(orgPath) && QFile::exists(dest))
    {
        QFile::remove(dest);
    }

    // rename tmp file to dest

    if (!renameFile(orgPath, dest))
    {
        return false;
    }

#ifndef Q_OS_WIN

    // restore permissions

    if (::chmod(dest.toUtf8().constData(), filePermissions) != 0)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to restore file permissions for file"
                                       << dest;
    }

#endif // Q_OS_WIN

    return true;
}

void DFileOperations::openFilesWithDefaultApplication(const QList<QUrl>& urls)
{
    if (urls.isEmpty())
    {
        return;
    }

    foreach (const QUrl& url, urls)
    {
        QDesktopServices::openUrl(url);
    }
}

QUrl DFileOperations::getUniqueFileUrl(const QUrl& orgUrl,
                                       bool* const newurl)
{
    if (newurl)
    {
        *newurl = false;
    }

    int counter = 0;
    QUrl destUrl(orgUrl);
    QFileInfo fi(destUrl.toLocalFile());
    QRegExp version(QLatin1String("(.+)_v(\\d+)"));
    QString completeBaseName = fi.completeBaseName();

    if (version.exactMatch(completeBaseName))
    {
        completeBaseName = version.cap(1);
        counter          = version.cap(2).toInt();
    }

    if (fi.exists())
    {
        bool fileFound = false;

        do
        {
            QFileInfo nfi(destUrl.toLocalFile());

            if (!nfi.exists())
            {
                fileFound = false;

                if (newurl)
                {
                    *newurl = true;
                }
            }
            else
            {
                fileFound = true;
                destUrl   = destUrl.adjusted(QUrl::RemoveFilename);
                destUrl.setPath(destUrl.path() + completeBaseName +
                                QString::fromUtf8("_v%1.").arg(++counter) + fi.completeSuffix());
            }
        }
        while (fileFound);
    }

    return destUrl;
}

QUrl DFileOperations::getUniqueFolderUrl(const QUrl& orgUrl)
{
    int counter              = 0;
    QUrl destUrl(orgUrl);
    QFileInfo fi(destUrl.toLocalFile());
    QRegExp version(QLatin1String("(.+)-(\\d+)"));
    QString completeFileName = fi.fileName();

    if (version.exactMatch(completeFileName))
    {
        completeFileName = version.cap(1);
        counter          = version.cap(2).toInt();
    }

    if (fi.exists())
    {
        bool fileFound = false;

        do
        {
            QFileInfo nfi(destUrl.toLocalFile());

            if (!nfi.exists())
            {
                fileFound = false;
            }
            else
            {
                fileFound = true;
                destUrl   = destUrl.adjusted(QUrl::RemoveFilename);
                destUrl.setPath(destUrl.path() + completeFileName +
                                QString::fromUtf8("-%1").arg(++counter));
            }
        }
        while (fileFound);
    }

    return destUrl;
}

void DFileOperations::openInFileManager(const QList<QUrl>& urls)
{
    if (urls.isEmpty())
    {
        return;
    }

    bool equal = true;
    QUrl first = urls.first();
    first      = first.adjusted(QUrl::RemoveFilename);

    foreach (const QUrl& url, urls)
    {
        if (first != url.adjusted(QUrl::RemoveFilename))
        {
            equal = false;
            break;
        }
    }

    QList<QUrl> fileUrls;

    if (equal)
    {
        fileUrls = urls;
    }
    else
    {
        fileUrls = QList<QUrl>() << urls.first();
    }

    QString path = fileUrls.first().toLocalFile();

#ifdef Q_OS_WIN

    QStringList args;
    QFileInfo info(path);

    if (!info.isDir())
    {
        args << QLatin1String("/select,");
    }

    args << QDir::toNativeSeparators(path);

    if (QProcess::startDetached(QLatin1String("explorer"), args))
    {
        return;
    }

#elif defined Q_OS_MACOS

    QStringList args;
    args << QLatin1String("-e");
    args << QLatin1String("tell application \"Finder\"");
    args << QLatin1String("-e");
    args << QLatin1String("activate");
    args << QLatin1String("-e");
    args << QString::fromUtf8("select POSIX file \"%1\"").arg(path);
    args << QLatin1String("-e");
    args << QLatin1String("end tell");
    args << QLatin1String("-e");
    args << QLatin1String("return");

    if (QProcess::execute(QLatin1String("/usr/bin/osascript"), args) == 0)
    {
        return;
    }

#elif HAVE_DBUS

    QDBusInterface iface(QLatin1String("org.freedesktop.FileManager1"),
                         QLatin1String("/org/freedesktop/FileManager1"),
                         QLatin1String("org.freedesktop.FileManager1"),
                         QDBusConnection::sessionBus());

    if (iface.isValid())
    {
        QStringList uris;

        foreach (const QUrl& url, fileUrls)
        {
            uris << url.toString();
        }

        iface.asyncCall(QLatin1String("ShowItems"), uris, QString());

        return;
    }

#endif

    QUrl url = fileUrls.first();
    url      = url.adjusted(QUrl::RemoveFilename |
                            QUrl::StripTrailingSlash);

    QDesktopServices::openUrl(url);
}

bool DFileOperations::copyFolderRecursively(const QString& srcPath,
                                            const QString& dstPath,
                                            const QString& itemId,
                                            bool* const cancel,
                                            bool  countTotal)
{
    QDir srcDir(srcPath);
    QString newCopyPath = dstPath + QLatin1Char('/') + srcDir.dirName();

    if (!srcDir.mkpath(newCopyPath))
    {
        return false;
    }

    if (countTotal && !itemId.isEmpty())
    {
        int count = 0;

        QDirIterator it(srcDir.path(), QDir::Files,
                                       QDirIterator::Subdirectories);

        while (it.hasNext())
        {
            it.next();
            ++count;
        }

        ProgressItem* const item = ProgressManager::instance()->findItembyId(itemId);

        if (item)
        {
            item->incTotalItems(count);
        }
    }

    foreach (const QFileInfo& fileInfo, srcDir.entryInfoList(QDir::Files))
    {
        QString copyPath = newCopyPath + QLatin1Char('/') + fileInfo.fileName();

        if (cancel && *cancel)
        {
            return false;
        }

        if (!copyFile(fileInfo.filePath(), copyPath))
        {
            return false;
        }

        if (!itemId.isEmpty())
        {
            ProgressItem* const item = ProgressManager::instance()->findItembyId(itemId);

            if (item)
            {
                item->advance(1);
            }
        }
    }

    foreach (const QFileInfo& fileInfo, srcDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (!copyFolderRecursively(fileInfo.filePath(), newCopyPath, itemId, cancel, false))
        {
            return false;
        }
    }

    return true;
}

bool DFileOperations::copyFiles(const QStringList& srcPaths,
                                const QString& dstPath)
{
    foreach (const QString& path, srcPaths)
    {
        QFileInfo fileInfo(path);
        QString copyPath = dstPath + QLatin1Char('/') + fileInfo.fileName();

        if (!copyFile(fileInfo.filePath(), copyPath))
        {
            return false;
        }
    }

    return true;
}

bool DFileOperations::renameFile(const QString& srcFile,
                                 const QString& dstFile)
{

#ifdef Q_OS_WIN64

    struct __utimbuf64 ut;
    struct __stat64    st;
    int stat = _wstat64((const wchar_t*)srcFile.utf16(), &st);

#elif defined Q_OS_WIN

    struct _utimbuf    ut;
    struct _stat       st;
    int stat = _wstat((const wchar_t*)srcFile.utf16(), &st);

#else

    struct utimbuf     ut;
    QT_STATBUF         st;
    int stat = QT_STAT(srcFile.toUtf8().constData(), &st);

#endif

    bool ret = QFile::rename(srcFile, dstFile);

    if (ret && (stat == 0))
    {
        ut.modtime = st.st_mtime;
        ut.actime  = st.st_atime;

#ifdef Q_OS_WIN64

        stat       = _wutime64((const wchar_t*)dstFile.utf16(), &ut);

#elif defined Q_OS_WIN

        stat       = _wutime((const wchar_t*)dstFile.utf16(), &ut);

#else

        stat       = ::utime(dstFile.toUtf8().constData(), &ut);

#endif

        if (stat != 0)
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to restore modification time for file"
                                           << dstFile;
        }
    }

    return ret;
}

bool DFileOperations::copyFile(const QString& srcFile,
                               const QString& dstFile)
{
    QString tmpFile;

    int path = dstFile.lastIndexOf(QLatin1Char('/'));
    int dot  = dstFile.lastIndexOf(QLatin1Char('.'));
    dot      = (path > dot) ? -1 : dot;
    int ext  = dstFile.length() - dot;
    tmpFile  = dstFile.left(dot);
    tmpFile += QLatin1String(".digikamtempfile");
    tmpFile += (dot < 0) ? QLatin1String(".tmp")
                         : dstFile.right(ext);

    bool ret = QFile::copy(srcFile, tmpFile);

    if (ret && !(ret = QFile::rename(tmpFile, dstFile)))
    {
        QFile::remove(tmpFile);
    }

    if (ret)
    {
        copyModificationTime(srcFile, dstFile);
    }

    return ret;
}

bool DFileOperations::copyModificationTime(const QString& srcFile,
                                           const QString& dstFile)
{

#ifdef Q_OS_WIN64

    struct __utimbuf64 ut;
    struct __stat64    st;
    int ret = _wstat64((const wchar_t*)srcFile.utf16(), &st);

#elif defined Q_OS_WIN

    struct _utimbuf    ut;
    struct _stat       st;
    int ret = _wstat((const wchar_t*)srcFile.utf16(), &st);

#else

    struct utimbuf     ut;
    QT_STATBUF         st;
    int ret = QT_STAT(srcFile.toUtf8().constData(), &st);

#endif

    if (ret == 0)
    {
        ut.modtime = st.st_mtime;
        ut.actime  = st.st_atime;

#ifdef Q_OS_WIN64

        ret        = _wutime64((const wchar_t*)dstFile.utf16(), &ut);

#elif defined Q_OS_WIN

        ret        = _wutime((const wchar_t*)dstFile.utf16(), &ut);

#else

        ret        = ::utime(dstFile.toUtf8().constData(), &ut);

#endif

    }

    if (ret != 0)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to restore modification time for file"
                                       << dstFile;
        return false;
    }

    return true;
}

bool DFileOperations::setModificationTime(const QString& srcFile,
                                          const QDateTime& dateTime)
{

#ifdef Q_OS_WIN64

    struct __utimbuf64 ut;
    struct __stat64    st;
    int ret = _wstat64((const wchar_t*)srcFile.utf16(), &st);

#elif defined Q_OS_WIN

    struct _utimbuf    ut;
    struct _stat       st;
    int ret = _wstat((const wchar_t*)srcFile.utf16(), &st);

#else

    struct utimbuf     ut;
    QT_STATBUF         st;
    int ret = QT_STAT(srcFile.toUtf8().constData(), &st);

#endif

    if (ret == 0)
    {
        ut.modtime = dateTime.toSecsSinceEpoch();
        ut.actime  = st.st_atime;

#ifdef Q_OS_WIN64

        ret        = _wutime64((const wchar_t*)srcFile.utf16(), &ut);

#elif defined Q_OS_WIN

        ret        = _wutime((const wchar_t*)srcFile.utf16(), &ut);

#else

        ret        = ::utime(srcFile.toUtf8().constData(), &ut);

#endif

    }

    if (ret != 0)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to set modification time for file"
                                       << srcFile;
        return false;
    }

    return true;
}

} // namespace Digikam
