/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-12-10
 * Description : misc file operation methods
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DFILE_OPERATIONS_H
#define DIGIKAM_DFILE_OPERATIONS_H

// Qt includes

#include <QUrl>
#include <QString>
#include <QDateTime>
#include <QStringList>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DFileOperations
{
public:

    /**
     * This method rename a local file 'orgPath' to 'destPath' with all ACL properties
     * restoration taken from 'source' file.
     * Return true if operation is completed.
     */
    static bool localFileRename(const QString& source,
                                const QString& orgPath,
                                const QString& destPath,
                                bool ignoreSettings = false);

    /**
     * Open file urls to default application relevant of
     * file type-mimes desktop configuration.
     */
    static void openFilesWithDefaultApplication(const QList<QUrl>& urls);

    /**
     * Get unique file url if file exist by appending a counter suffix
     * or return original url.
     */
    static QUrl getUniqueFileUrl(const QUrl& orgUrl, bool* const newurl = nullptr);

    /**
     * Get unique folder url if folder exist by appending a counter suffix
     * or return original url.
     */
    static QUrl getUniqueFolderUrl(const QUrl& orgUrl);

    /**
     * Open system file manager and select the item.
     */
    static void openInFileManager(const QList<QUrl>& urls);

    /**
     * Copy recursively a directory contents to another one.
     */
    static bool copyFolderRecursively(const QString& srcPath,
                                      const QString& dstPath,
                                      const QString& itemId = QString(),
                                      bool* const cancel = nullptr,
                                      bool  countTotal = true);

    /**
     * Copy a list of files to another place.
     */
    static bool copyFiles(const QStringList& srcPaths,
                          const QString& dstPath);

    /**
     * Rename or move file and keep the source file modification time.
     */
    static bool renameFile(const QString& srcFile,
                           const QString& dstFile);

    /**
     * Copy file and keep the source file modification time.
     */
    static bool copyFile(const QString& srcFile,
                         const QString& dstFile);

    /**
     * Copy file modification time from source to destination file.
     */
    static bool copyModificationTime(const QString& srcFile,
                                     const QString& dstFile);
    /**
     * Set file modification time from QDateTime.
     * Keep access time from source file.
     */
    static bool setModificationTime(const QString& srcFile,
                                    const QDateTime& dateTime);
};

} // namespace Digikam

#endif // DIGIKAM_DFILE_OPERATIONS_H
