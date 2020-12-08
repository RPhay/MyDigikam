/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-17
 * Description : low level files management interface.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2018      by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "dio.h"

// Qt includes

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QAbstractButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "iteminfo.h"
#include "diofinders.h"
#include "albummanager.h"
#include "tagscache.h"
#include "coredb.h"
#include "coredbaccess.h"
#include "album.h"
#include "dmetadata.h"
#include "metaenginesettings.h"
#include "scancontroller.h"
#include "thumbsdb.h"
#include "thumbsdbaccess.h"
#include "iojobsmanager.h"
#include "collectionmanager.h"
#include "collectionlocation.h"
#include "dnotificationwrapper.h"
#include "loadingcacheinterface.h"
#include "progressmanager.h"
#include "digikamapp.h"
#include "iojobdata.h"

namespace Digikam
{

class Q_DECL_HIDDEN DIOCreator
{
public:

    DIO object;
};

Q_GLOBAL_STATIC(DIOCreator, creator)

// ------------------------------------------------------------------------------------------------

DIO* DIO::instance()
{
    return &creator->object;
}

DIO::DIO()
{
    m_processingCount = 0;
}

DIO::~DIO()
{
}

void DIO::cleanUp()
{
}

bool DIO::itemsUnderProcessing()
{
    return instance()->m_processingCount;
}

// Album -> Album -----------------------------------------------------

void DIO::copy(PAlbum* const src, PAlbum* const dest)
{
    if (!src || !dest)
    {
        return;
    }

    instance()->processJob(new IOJobData(IOJobData::CopyAlbum, src, dest));
}

void DIO::move(PAlbum* const src, PAlbum* const dest)
{
    if (!src || !dest)
    {
        return;
    }

#ifdef Q_OS_WIN

    AlbumManager::instance()->removeWatchedPAlbums(src);

#endif

    instance()->processJob(new IOJobData(IOJobData::MoveAlbum, src, dest));
}

// Images -> Album ----------------------------------------------------

void DIO::copy(const QList<ItemInfo>& infos, PAlbum* const dest)
{
    if (!dest)
    {
        return;
    }

    instance()->processJob(new IOJobData(IOJobData::CopyImage, infos, dest));
}

void DIO::move(const QList<ItemInfo>& infos, PAlbum* const dest)
{
    if (!dest)
    {
        return;
    }

    instance()->processJob(new IOJobData(IOJobData::MoveImage, infos, dest));
}

// External files -> album --------------------------------------------

void DIO::copy(const QUrl& src, PAlbum* const dest)
{
    copy(QList<QUrl>() << src, dest);
}

void DIO::copy(const QList<QUrl>& srcList, PAlbum* const dest)
{
    if (!dest)
    {
        return;
    }

    instance()->processJob(new IOJobData(IOJobData::CopyFiles, srcList, dest));
}

void DIO::move(const QUrl& src, PAlbum* const dest)
{
    move(QList<QUrl>() << src, dest);
}

void DIO::move(const QList<QUrl>& srcList, PAlbum* const dest)
{
    if (!dest)
    {
        return;
    }

    instance()->processJob(new IOJobData(IOJobData::MoveFiles, srcList, dest));
}

// Images -> External -------------------------------------------------

void DIO::copy(const QList<ItemInfo>& infos, const QUrl& dest)
{
    instance()->processJob(new IOJobData(IOJobData::CopyToExt, infos, dest));
}

// Rename -------------------------------------------------------------

void DIO::rename(const QUrl& src, const QString& newName, bool overwrite)
{
    if (src.isEmpty() || newName.isEmpty())
    {
        return;
    }

    ItemInfo info = ItemInfo::fromUrl(src);

    instance()->processJob(new IOJobData(IOJobData::Rename, info, newName, overwrite));
}

// Delete -------------------------------------------------------------

void DIO::del(const QList<ItemInfo>& infos, bool useTrash)
{
    instance()->processJob(new IOJobData(useTrash ? IOJobData::Trash
                                                  : IOJobData::Delete, infos));
}

void DIO::del(const ItemInfo& info, bool useTrash)
{
    del(QList<ItemInfo>() << info, useTrash);
}

void DIO::del(PAlbum* const album, bool useTrash)
{
    if (!album)
    {
        return;
    }

#ifdef Q_OS_WIN

    AlbumManager::instance()->removeWatchedPAlbums(album);

#endif

    instance()->createJob(new IOJobData(useTrash ? IOJobData::Trash
                                                 : IOJobData::Delete, album));
}

// Restore Trash ------------------------------------------------------

void DIO::restoreTrash(const DTrashItemInfoList& infos)
{
    instance()->createJob(new IOJobData(IOJobData::Restore, infos));
}

// Empty Trash --------------------------------------------------------

void DIO::emptyTrash(const DTrashItemInfoList& infos)
{
    instance()->createJob(new IOJobData(IOJobData::Empty, infos));
}

// ------------------------------------------------------------------------------------------------

void DIO::processJob(IOJobData* const data)
{
    const int operation = data->operation();

    if ((operation == IOJobData::CopyImage) || (operation == IOJobData::MoveImage))
    {
        // this is a fast db operation, do here

        GroupedImagesFinder finder(data->itemInfos());
        data->setItemInfos(finder.infos);

        QStringList      filenames;
        QList<qlonglong> ids;

        foreach (const ItemInfo& info, data->itemInfos())
        {
            filenames << info.name();
            ids << info.id();
        }

        ScanController::instance()->hintAtMoveOrCopyOfItems(ids, data->destAlbum(), filenames);
    }
    else if ((operation == IOJobData::CopyAlbum) || (operation == IOJobData::MoveAlbum))
    {
        ScanController::instance()->hintAtMoveOrCopyOfAlbum(data->srcAlbum(), data->destAlbum());
        createJob(data);

        return;
    }
    else if ((operation == IOJobData::Delete) || (operation == IOJobData::Trash))
    {
        qCDebug(DIGIKAM_DATABASE_LOG) << "Number of files to be deleted:" << data->sourceUrls().count();
    }

    SidecarFinder finder(data->sourceUrls());
    data->setSourceUrls(finder.localFiles);

    if (operation == IOJobData::Rename)
    {
        if (!data->itemInfos().isEmpty())
        {
            ItemInfo info       = data->itemInfos().constFirst();
            PAlbum* const album = AlbumManager::instance()->findPAlbum(info.albumId());

            if (album)
            {
                ScanController::instance()->hintAtMoveOrCopyOfItem(info.id(), album,
                                                                   data->destUrl().fileName());
            }

            for (int i = 0 ; i < finder.localFiles.length() ; ++i)
            {
                if (finder.localFileModes.at(i))
                {
                    data->setDestUrl(finder.localFiles.at(i),
                                     QUrl::fromLocalFile(data->destUrl().toLocalFile() +
                                                         finder.localFileSuffixes.at(i)));
                }
                else
                {
                    QFileInfo basInfo(data->destUrl().toLocalFile());

                    data->setDestUrl(finder.localFiles.at(i),
                                     QUrl::fromLocalFile(basInfo.path()             +
                                                         QLatin1Char('/')           +
                                                         basInfo.completeBaseName() +
                                                         finder.localFileSuffixes.at(i)));
                }
            }
        }
    }

    createJob(data);
}

void DIO::createJob(IOJobData* const data)
{
    if (data->sourceUrls().isEmpty())
    {
        delete data;

        return;
    }

    const int operation = data->operation();

    if ((operation == IOJobData::CopyImage) || (operation == IOJobData::CopyAlbum) ||
        (operation == IOJobData::CopyFiles) || (operation == IOJobData::CopyToExt) ||
        (operation == IOJobData::MoveImage) || (operation == IOJobData::MoveAlbum) ||
        (operation == IOJobData::MoveFiles))
    {
        QDir dir(data->destUrl().toLocalFile());
        const QStringList& dirList = dir.entryList(QDir::Dirs    |
                                                   QDir::Files   |
                                                   QDir::NoDotAndDotDot);

        foreach (const QUrl& url, data->sourceUrls())
        {
            if (dirList.contains(url.adjusted(QUrl::StripTrailingSlash).fileName()))
            {
                QPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Warning,
                        i18n("File conflict"),
                        i18n("Files or folders with the same name already exist "
                              "in the target folder.\n\n"
                              "What action is applied in the event of a file conflict?"),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Ok | QMessageBox::Cancel,
                        qApp->activeWindow());

                msgBox->button(QMessageBox::Yes)->setText(i18n("Rename automatically"));
                msgBox->button(QMessageBox::Yes)->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
                msgBox->button(QMessageBox::No)->setText(i18n("Overwrite automatically"));
                msgBox->button(QMessageBox::No)->setIcon(QIcon::fromTheme(QLatin1String("edit-copy")));
                msgBox->button(QMessageBox::Ok)->setText(i18n("Continue"));
                msgBox->button(QMessageBox::Ok)->setIcon(QIcon::fromTheme(QLatin1String("go-next")));

                if ((operation == IOJobData::CopyAlbum) || (operation == IOJobData::MoveAlbum))
                {
                    msgBox->button(QMessageBox::No)->hide();
                }

                int result = msgBox->exec();
                delete msgBox;

                if      (result == QMessageBox::Cancel)
                {
                    delete data;

                    return;
                }
                else if (result == QMessageBox::Yes)
                {
                    data->setFileConflict(IOJobData::AutoRename);
                }
                else if (result == QMessageBox::No)
                {
                    data->setFileConflict(IOJobData::Overwrite);
                }

                break;
            }
        }
    }

    ProgressItem* item = nullptr;
    QString itemString = getItemString(data);

    if (!itemString.isEmpty())
    {
        item = ProgressManager::instance()->createProgressItem(itemString,
                                                               QString(), true, false);
        item->setTotalItems(data->sourceUrls().count());
        data->setProgressId(item->id());
    }

    IOJobsThread* const jobThread = IOJobsManager::instance()->startIOJobs(data);

    connect(jobThread, SIGNAL(signalOneProccessed(QUrl)),
            this, SLOT(slotOneProccessed(QUrl)));

    connect(jobThread, SIGNAL(signalFinished()),
            this, SLOT(slotResult()));

    if (operation == IOJobData::Rename)
    {
        connect(jobThread, SIGNAL(signalRenameFailed(QUrl)),
                this, SIGNAL(signalRenameFailed(QUrl)));

        connect(jobThread, SIGNAL(signalFinished()),
                this, SIGNAL(signalRenameFinished()));
    }

    if ((operation == IOJobData::Empty) ||
        (operation == IOJobData::Restore))
    {
        connect(jobThread, SIGNAL(signalFinished()),
                this, SIGNAL(signalTrashFinished()));
    }

    if (item)
    {
        connect(item, SIGNAL(progressItemCanceled(ProgressItem*)),
                jobThread, SLOT(slotCancel()));

        connect(item, SIGNAL(progressItemCanceled(ProgressItem*)),
                this, SLOT(slotCancel(ProgressItem*)));
    }

    ++m_processingCount;
}

void DIO::slotResult()
{
    IOJobsThread* const jobThread = dynamic_cast<IOJobsThread*>(sender());

    if (!jobThread || !jobThread->jobData())
    {
        return;
    }

    IOJobData* const data = jobThread->jobData();

    if (jobThread->hasErrors() && data->operation() != IOJobData::Rename)
    {
        // Pop-up a message about the error.

        QString errors = jobThread->errorsList().join(QLatin1Char('\n'));
        DNotificationWrapper(QString(), errors, DigikamApp::instance(),
                             DigikamApp::instance()->windowTitle());
    }

    if (m_processingCount)
    {
        --m_processingCount;
    }

    slotCancel(getProgressItem(data));
}

void DIO::slotOneProccessed(const QUrl& url)
{
    IOJobsThread* const jobThread = dynamic_cast<IOJobsThread*>(sender());

    if (!jobThread || !jobThread->jobData())
    {
        return;
    }

    IOJobData* const data = jobThread->jobData();
    const int operation   = data->operation();

    if (operation == IOJobData::MoveImage)
    {
        ItemInfo info = data->findItemInfo(url);

        if (!info.isNull() && data->destAlbum())
        {
            QString destName = info.name();

            if (!data->destUrl(url).fileName().isEmpty())
            {
                destName = data->destUrl(url).fileName();
            }

            CoreDbAccess().db()->moveItem(info.albumId(), info.name(),
                                          data->destAlbum()->id(), destName);
        }
    }
    else if (operation == IOJobData::Delete)
    {
        // Mark the images as obsolete and remove them
        // from their album and from the grouped

        PAlbum* const album = data->srcAlbum();

        if (album && (album->fileUrl() == url))
        {
            // get all deleted albums

            CoreDbAccess access;
            QList<int> albumsToDelete;
            QList<qlonglong> imagesToRemove;

            addAlbumChildrenToList(albumsToDelete, album);

            foreach (int albumId, albumsToDelete)
            {
                imagesToRemove << access.db()->getItemIDsInAlbum(albumId);
            }

            foreach (const qlonglong& imageId, imagesToRemove)
            {
                access.db()->removeAllImageRelationsFrom(imageId,
                                                         DatabaseRelation::Grouped);
            }

            access.db()->removeItemsPermanently(imagesToRemove, albumsToDelete);
        }
        else
        {
            ItemInfo info = data->findItemInfo(url);

            if (!info.isNull())
            {
                int originalVersionTag    = TagsCache::instance()->getOrCreateInternalTag(InternalTagName::originalVersion());
                int needTaggingTag        = TagsCache::instance()->getOrCreateInternalTag(InternalTagName::needTaggingHistoryGraph());
                QList<qlonglong> imageIds = CoreDbAccess().db()->getImagesRelatedFrom(info.id(), DatabaseRelation::DerivedFrom);

                CoreDbAccess access;

                foreach (const qlonglong& id, imageIds)
                {
                    access.db()->removeItemTag(id, originalVersionTag);
                    access.db()->addItemTag(id, needTaggingTag);
                }

                access.db()->removeAllImageRelationsFrom(info.id(),
                                                         DatabaseRelation::Grouped);

                access.db()->removeItemsPermanently(QList<qlonglong>() << info.id(),
                                                    QList<int>() << info.albumId());
            }
        }
    }
    else if (operation == IOJobData::Trash)
    {
        ItemInfo info = data->findItemInfo(url);

        if (!info.isNull())
        {
            int originalVersionTag    = TagsCache::instance()->getOrCreateInternalTag(InternalTagName::originalVersion());
            int needTaggingTag        = TagsCache::instance()->getOrCreateInternalTag(InternalTagName::needTaggingHistoryGraph());
            QList<qlonglong> imageIds = CoreDbAccess().db()->getImagesRelatedFrom(info.id(), DatabaseRelation::DerivedFrom);

            CoreDbAccess access;

            foreach (const qlonglong& id, imageIds)
            {
                access.db()->removeItemTag(id, originalVersionTag);
                access.db()->addItemTag(id, needTaggingTag);
            }

            access.db()->removeAllImageRelationsFrom(info.id(),
                                                     DatabaseRelation::Grouped);

            access.db()->removeItems(QList<qlonglong>() << info.id(),
                                     QList<int>() << info.albumId());
        }
    }
    else if (operation == IOJobData::Rename)
    {
        ItemInfo info = data->findItemInfo(url);

        if (!info.isNull())
        {
            QString oldPath = url.toLocalFile();
            QString newName = data->destUrl(url).fileName();
            QString newPath = data->destUrl(url).toLocalFile();

            if (data->fileConflict() == IOJobData::Overwrite)
            {
                ThumbsDbAccess().db()->removeByFilePath(newPath);
                LoadingCacheInterface::fileChanged(newPath, false);
                CoreDbAccess().db()->deleteItem(info.albumId(), newName);
            }

            ThumbsDbAccess().db()->renameByFilePath(oldPath, newPath);

            // Remove old thumbnails and images from the cache

            LoadingCacheInterface::fileChanged(oldPath, false);

            // Rename in ItemInfo and database

            info.setName(newName);
        }
    }

    // Scan folders for changes

    QStringList scanPaths;

    if ((operation == IOJobData::Trash)   ||
        (operation == IOJobData::Delete)  ||
        (operation == IOJobData::MoveAlbum))
    {
        PAlbum* const album = data->srcAlbum();

        if (album)
        {
            PAlbum* const parent = dynamic_cast<PAlbum*>(album->parent());

            if (parent)
            {
                scanPaths << parent->fileUrl().toLocalFile();
            }
        }

        if (scanPaths.isEmpty())
        {
            scanPaths << url.adjusted(QUrl::RemoveFilename).toLocalFile();
        }
    }

    if ((operation == IOJobData::CopyImage) || (operation == IOJobData::CopyAlbum) ||
        (operation == IOJobData::CopyFiles) || (operation == IOJobData::MoveImage) ||
        (operation == IOJobData::MoveAlbum) || (operation == IOJobData::MoveFiles))
    {
        scanPaths << data->destUrl().toLocalFile();
    }

    if (operation == IOJobData::CopyToExt)
    {
        CollectionLocation location = CollectionManager::instance()->locationForUrl(data->destUrl());

        if (!location.isNull())
        {
            scanPaths << data->destUrl().toLocalFile();
        }
    }

    if (operation == IOJobData::Restore)
    {
        scanPaths << url.adjusted(QUrl::RemoveFilename).toLocalFile();
    }

    foreach (const QString& scanPath, scanPaths)
    {
        ScanController::instance()->scheduleCollectionScanRelaxed(scanPath);
    }

    ProgressItem* const item = getProgressItem(data);

    if (item)
    {
        item->advance(1);
    }
}

QString DIO::getItemString(IOJobData* const data) const
{
    switch (data->operation())
    {
        case IOJobData::CopyAlbum:
            return i18n("Copy Album");

        case IOJobData::CopyImage:
            return i18n("Copy Images");

        case IOJobData::CopyFiles:
        case IOJobData::CopyToExt:
            return i18n("Copy Files");

        case IOJobData::MoveAlbum:
            return i18n("Move Album");

        case IOJobData::MoveImage:
            return i18n("Move Images");

        case IOJobData::MoveFiles:
            return i18n("Move Files");

        case IOJobData::Delete:
            return i18n("Delete");

        case IOJobData::Trash:
            return i18n("Trash");

        case IOJobData::Restore:
            return i18n("Restore Trash");

        case IOJobData::Empty:
            return i18n("Empty Trash");

        default:
            break;
    }

    return QString();
}

ProgressItem* DIO::getProgressItem(IOJobData* const data) const
{
    QString itemId = data->getProgressId();

    if (itemId.isEmpty())
    {
        return nullptr;
    }

    return ProgressManager::instance()->findItembyId(itemId);
}

void DIO::slotCancel(ProgressItem* item)
{
    if (item)
    {
        item->setComplete();
    }
}

void DIO::addAlbumChildrenToList(QList<int>& list, Album* const album)
{
    // simple recursive helper function

    if (album)
    {
        if (!list.contains(album->id()))
        {
            list.append(album->id());
        }

        AlbumIterator it(album);

        while (it.current())
        {
            addAlbumChildrenToList(list, *it);
            ++it;
        }
    }
}

} // namespace Digikam
