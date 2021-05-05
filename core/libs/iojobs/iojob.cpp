/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-15
 * Description : IO Jobs for file systems jobs
 *
 * Copyright (C) 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2018 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "iojob.h"

// Qt includes

#include <QDir>
#include <QFile>
#include <QDirIterator>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "iteminfo.h"
#include "dtrash.h"
#include "coredb.h"
#include "coredbaccess.h"
#include "albummanager.h"
#include "dfileoperations.h"
#include "coredboperationgroup.h"

namespace Digikam
{

IOJob::IOJob()
{
}

// --------------------------------------------

CopyOrMoveJob::CopyOrMoveJob(IOJobData* const data)
    : m_data(data)
{
}

void CopyOrMoveJob::run()
{
    while (m_data && !m_cancel)
    {
        QUrl srcUrl = m_data->getNextUrl();

        if (srcUrl.isEmpty())
        {
            break;
        }

        QFileInfo srcInfo(srcUrl.adjusted(QUrl::StripTrailingSlash).toLocalFile());
        QDir dstDir(m_data->destUrl().toLocalFile());
        QString srcName = srcInfo.fileName();

        if (!srcInfo.exists())
        {
            emit signalError(i18n("File/Folder %1 does not exist anymore", srcName));
            continue;
        }

        if (!dstDir.exists())
        {
            emit signalError(i18n("Album %1 does not exist anymore", dstDir.dirName()));
            continue;
        }

        // Checking if there is a file with the same name in destination folder

        QString destenation = dstDir.path() + QLatin1Char('/') + srcName;

        if (QFileInfo::exists(destenation))
        {
            if      (m_data->fileConflict() == IOJobData::Overwrite)
            {
                if (srcInfo.isDir())
                {
                    continue;
                }
                else
                {
                    if (srcInfo.filePath() == destenation)
                    {
                        continue;
                    }

                    if      (m_data->operation() == IOJobData::CopyToExt)
                    {
                        if (!QFile::remove(destenation))
                        {
                            emit signalError(i18n("Could not overwrite image %1",
                                             srcName));

                            continue;
                        }
                    }
                    else if (!DTrash::deleteImage(destenation, m_data->jobTime()))
                    {
                        emit signalError(i18n("Could not move image %1 to collection trash",
                                              srcName));

                        continue;
                    }
                }
            }
            else if (m_data->fileConflict() == IOJobData::AutoRename)
            {
                QUrl destUrl = QUrl::fromLocalFile(destenation);

                if (srcInfo.isDir())
                {
                    destenation = DFileOperations::getUniqueFolderUrl(destUrl).toLocalFile();
                }
                else
                {
                    QUrl renamed = DFileOperations::getUniqueFileUrl(destUrl);
                    destenation  = renamed.toLocalFile();
                    m_data->setDestUrl(srcUrl, renamed);
                }
            }
            else
            {
                emit signalError(i18n("A file or folder named %1 already exists in %2",
                                      srcName, QDir::toNativeSeparators(dstDir.path())));

                continue;
            }
        }

        if ((m_data->operation() == IOJobData::MoveAlbum) ||
            (m_data->operation() == IOJobData::MoveImage) ||
            (m_data->operation() == IOJobData::MoveFiles))
        {
            if (srcInfo.isDir())
            {
                QDir srcDir(srcInfo.filePath());

                if (!srcDir.rename(srcDir.path(), destenation))
                {
                    // If QDir::rename fails, try copy and remove.

                    if      (!DFileOperations::copyFolderRecursively(srcDir.path(), dstDir.path(),
                                                                     m_data->getProgressId(), &m_cancel))
                    {
                        emit signalOneProccessed(srcUrl);

                        if (m_cancel)
                        {
                            break;
                        }

                        emit signalError(i18n("Could not move folder %1 to album %2",
                                              srcName, QDir::toNativeSeparators(dstDir.path())));

                        continue;
                    }
                    else if (!srcDir.removeRecursively())
                    {
                        emit signalError(i18n("Could not move folder %1 to album %2. "
                                              "The folder %1 was copied as well to album %2",
                                              srcName, QDir::toNativeSeparators(dstDir.path())));
                    }
                }
            }
            else
            {
                if (!DFileOperations::renameFile(srcInfo.filePath(), destenation))
                {
                    emit signalError(i18n("Could not move file %1 to album %2",
                                          srcName, QDir::toNativeSeparators(dstDir.path())));

                    continue;
                }
           }
        }
        else
        {
            if (srcInfo.isDir())
            {
                QDir srcDir(srcInfo.filePath());

                if (!DFileOperations::copyFolderRecursively(srcDir.path(), dstDir.path(),
                                                            m_data->getProgressId(), &m_cancel))
                {
                    emit signalOneProccessed(srcUrl);

                    if (m_cancel)
                    {
                        break;
                    }

                    emit signalError(i18n("Could not copy folder %1 to album %2",
                                          srcName, QDir::toNativeSeparators(dstDir.path())));

                    continue;
                }
            }
            else
            {
                if (!DFileOperations::copyFile(srcInfo.filePath(), destenation))
                {
                    if (m_data->operation() == IOJobData::CopyToExt)
                    {
                        emit signalError(i18n("Could not copy file %1 to folder %2",
                                         srcName, QDir::toNativeSeparators(dstDir.path())));
                    }
                    else
                    {
                        emit signalError(i18n("Could not copy file %1 to album %2",
                                         srcName, QDir::toNativeSeparators(dstDir.path())));
                    }

                    continue;
                }
            }
        }

        emit signalOneProccessed(srcUrl);
    }

    emit signalDone();
}

// --------------------------------------------

DeleteJob::DeleteJob(IOJobData* const data)
    : m_data(data)
{
}

void DeleteJob::run()
{
    while (m_data && !m_cancel)
    {
        QUrl deleteUrl = m_data->getNextUrl();

        if (deleteUrl.isEmpty())
        {
            break;
        }

        bool useTrash = (m_data->operation() == IOJobData::Trash);

        QFileInfo fileInfo(deleteUrl.toLocalFile());
        qCDebug(DIGIKAM_IOJOB_LOG) << "Deleting:   " << fileInfo.filePath();
        qCDebug(DIGIKAM_IOJOB_LOG) << "File exists?" << fileInfo.exists();
        qCDebug(DIGIKAM_IOJOB_LOG) << "Is to trash?" << useTrash;

        if (!fileInfo.exists())
        {
            emit signalError(i18n("File/Folder %1 does not exist",
                                  QDir::toNativeSeparators(fileInfo.filePath())));

            continue;
        }

        if (useTrash)
        {
            if (fileInfo.isDir())
            {
                if (!DTrash::deleteDirRecursivley(deleteUrl.toLocalFile(), m_data->jobTime()))
                {
                    emit signalError(i18n("Could not move folder %1 to collection trash",
                                          QDir::toNativeSeparators(fileInfo.path())));

                    continue;
                }
            }
            else
            {
                if (!DTrash::deleteImage(deleteUrl.toLocalFile(), m_data->jobTime()))
                {
                    emit signalError(i18n("Could not move image %1 to collection trash",
                                          QDir::toNativeSeparators(fileInfo.filePath())));

                    continue;
                }
            }
        }
        else
        {
            if (fileInfo.isDir())
            {
                QDir dir(fileInfo.filePath());

                if (!dir.removeRecursively())
                {
                    emit signalError(i18n("Album %1 could not be removed",
                                          QDir::toNativeSeparators(fileInfo.path())));

                    continue;
                }
            }
            else
            {
                QFile file(fileInfo.filePath());

                if (!file.remove())
                {
                    emit signalError(i18n("Image %1 could not be removed",
                                          QDir::toNativeSeparators(fileInfo.filePath())));

                    continue;
                }
            }
        }

        emit signalOneProccessed(deleteUrl);
    }

    emit signalDone();
}

// --------------------------------------------

RenameFileJob::RenameFileJob(IOJobData* const data)
    : m_data(data)
{
}

void RenameFileJob::run()
{
    while (m_data && !m_cancel)
    {
        QUrl renameUrl = m_data->getNextUrl();

        if (renameUrl.isEmpty())
        {
            break;
        }

        QUrl destUrl = m_data->destUrl(renameUrl);
        QFileInfo fileInfo(destUrl.toLocalFile());

        QDir dir(fileInfo.dir());
        const QStringList& dirList = dir.entryList(QDir::Dirs    |
                                                   QDir::Files   |
                                                   QDir::NoDotAndDotDot);

        if (dirList.contains(fileInfo.fileName()))
        {
            if (m_data->fileConflict() == IOJobData::Overwrite)
            {
                if (!DTrash::deleteImage(destUrl.toLocalFile(), m_data->jobTime()))
                {
                    emit signalError(i18n("Could not move image %1 to collection trash",
                                          QDir::toNativeSeparators(destUrl.toLocalFile())));

                    emit signalRenameFailed(renameUrl);
                    continue;
                }
            }
            else
            {
                qCDebug(DIGIKAM_IOJOB_LOG) << "File with the same name exists!";
                emit signalError(i18n("Image with the same name %1 already there",
                                      QDir::toNativeSeparators(destUrl.toLocalFile())));

                emit signalRenameFailed(renameUrl);
                continue;
            }
        }

        qCDebug(DIGIKAM_IOJOB_LOG) << "Trying to rename"
                                   << renameUrl.toLocalFile() << "to"
                                   << destUrl.toLocalFile();

        if (!DFileOperations::renameFile(renameUrl.toLocalFile(), destUrl.toLocalFile()))
        {
            qCDebug(DIGIKAM_IOJOB_LOG) << "File could not be renamed!";
            emit signalError(i18n("Image %1 could not be renamed",
                                  QDir::toNativeSeparators(renameUrl.toLocalFile())));

            emit signalRenameFailed(renameUrl);
            continue;
        }

        emit signalOneProccessed(renameUrl);
    }

    emit signalDone();
}

// ----------------------------------------------

DTrashItemsListingJob::DTrashItemsListingJob(const QString& collectionPath)
    : m_collectionPath(collectionPath)
{
}

void DTrashItemsListingJob::run()
{
    DTrashItemInfo itemInfo;

    QString collectionTrashFilesPath = m_collectionPath + QLatin1Char('/') + DTrash::TRASH_FOLDER +
                                       QLatin1Char('/') + DTrash::FILES_FOLDER;

    qCDebug(DIGIKAM_IOJOB_LOG) << "Collection trash files path:" << collectionTrashFilesPath;

    QDir filesDir(collectionTrashFilesPath);

    foreach (const QFileInfo& fileInfo, filesDir.entryInfoList(QDir::Files))
    {
        qCDebug(DIGIKAM_IOJOB_LOG) << "File in trash:" << fileInfo.filePath();
        itemInfo.trashPath = fileInfo.filePath();

        DTrash::extractJsonForItem(m_collectionPath, fileInfo.baseName(), itemInfo);

        emit trashItemInfo(itemInfo);
    }

    emit signalDone();
}

// ----------------------------------------------

RestoreDTrashItemsJob::RestoreDTrashItemsJob(IOJobData* const data)
    : m_data(data)
{
}

void RestoreDTrashItemsJob::run()
{
    if (!m_data)
    {
        return;
    }

    foreach (const DTrashItemInfo& item, m_data->trashItems())
    {
        QUrl srcToRename = QUrl::fromLocalFile(item.collectionPath);
        QUrl newName     = DFileOperations::getUniqueFileUrl(srcToRename);

        QFileInfo fi(item.collectionPath);

        if (!fi.dir().exists())
        {
            fi.dir().mkpath(fi.dir().path());
        }

        if (!QFile::rename(item.trashPath, newName.toLocalFile()))
        {
            emit signalError(i18n("Could not restore file %1 from trash",
                                  QDir::toNativeSeparators(newName.toLocalFile())));
        }
        else
        {
            QFile::remove(item.jsonFilePath);
        }

        emit signalOneProccessed(newName);
    }

    emit signalDone();
}

// ----------------------------------------------

EmptyDTrashItemsJob::EmptyDTrashItemsJob(IOJobData* const data)
    : m_data(data)
{
}

void EmptyDTrashItemsJob::run()
{
    if (!m_data)
    {
        return;
    }

    QList<int> albumsFromImages;
    QList<qlonglong> imagesToRemove;

    foreach (const DTrashItemInfo& item, m_data->trashItems())
    {
        QFile::remove(item.trashPath);
        QFile::remove(item.jsonFilePath);

        imagesToRemove   << item.imageId;
        albumsFromImages << ItemInfo(item.imageId).albumId();

        emit signalOneProccessed(QUrl());
    }

    {
        CoreDbOperationGroup group;
        group.setMaximumTime(200);

        CoreDbAccess().db()->removeItemsPermanently(imagesToRemove, albumsFromImages);
    }

    emit signalDone();
}

} // namespace Digikam
