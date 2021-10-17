/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : stand alone digiKam image editor - Open files
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfoto_p.h"

namespace ShowFoto
{

void Showfoto::slotOpenFile()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    QList<QUrl> urls = Digikam::ImageDialog::getImageURLs(this, d->lastOpenedDirectory);

    if      (urls.count() > 1)
    {
        slotClearThumbBar();

        openUrls(urls);
        emit signalInfoList(d->infoList);

        slotOpenUrl(d->thumbBar->currentInfo());
        toggleNavigation(1);
    }
    else if (urls.count() == 1)
    {
        slotClearThumbBar();

        openFolder(urls.first().adjusted(QUrl::RemoveFilename));
        emit signalInfoList(d->infoList);

        slotOpenUrl(d->thumbBar->findItemByUrl(urls.first()));
        d->thumbBar->setCurrentUrl(urls.first());
    }
}

void Showfoto::slotOpenFolder()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    QUrl url = DFileDialog::getExistingDirectoryUrl(this, i18n("Open Images From Folder"),
                                                    d->lastOpenedDirectory);
    if (!url.isEmpty())
    {
        slotClearThumbBar();

        openFolder(url);
        emit signalInfoList(d->infoList);

        slotOpenUrl(d->thumbBar->currentInfo());
        toggleNavigation(1);
    }
}

void Showfoto::slotOpenFolderFromPath(const QString& path)
{
    qCDebug(DIGIKAM_SHOWFOTO_LOG) << "Open folder from path =" << path;

    QFileInfo inf(path);
    slotClearThumbBar();

    if      (inf.isFile())
    {
        slotDroppedUrls(QList<QUrl>() << QUrl::fromLocalFile(inf.absolutePath()), false, QUrl());
        d->thumbBar->setCurrentUrl(QUrl::fromLocalFile(path));
        slotOpenUrl(d->thumbBar->currentInfo());
    }
    else if (inf.isDir())
    {
        QString dpath(path.endsWith(QLatin1Char('/')) ? path : path + QLatin1Char('/'));
        slotDroppedUrls(QList<QUrl>() << QUrl::fromLocalFile(dpath), false, QUrl());
        QList<QUrl> urls = d->thumbBar->urls();

        if (!urls.isEmpty())
        {
            d->thumbBar->setCurrentUrl(urls.first());
            slotOpenUrl(d->thumbBar->currentInfo());
        }
    }
    else
    {
        return;
    }

    applySortSettings();
}

void Showfoto::openUrls(const QList<QUrl>& urls)
{
    if (urls.isEmpty())
    {
        return;
    }

    ShowfotoItemInfo iteminfo;
    QScopedPointer<DMetadata> meta(new DMetadata);

    for (QList<QUrl>::const_iterator it = urls.constBegin() ; it != urls.constEnd() ; ++it)
    {
        QFileInfo fi((*it).toLocalFile());
        iteminfo.name      = fi.fileName();
        iteminfo.mime      = fi.suffix();
        iteminfo.size      = fi.size();
        iteminfo.folder    = fi.path();
        iteminfo.url       = QUrl::fromLocalFile(fi.filePath());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))

        iteminfo.dtime     = fi.birthTime();

#else

        iteminfo.dtime     = fi.created();

#endif

        meta->load(fi.filePath());
        iteminfo.ctime     = meta->getItemDateTime();
        iteminfo.width     = meta->getItemDimensions().width();
        iteminfo.height    = meta->getItemDimensions().height();
        iteminfo.photoInfo = meta->getPhotographInformation();

        if (!d->infoList.contains(iteminfo))
        {
            d->infoList << iteminfo;
        }
    }
}

void Showfoto::openFolder(const QUrl& url)
{
    if (!url.isValid() || !url.isLocalFile())
    {
        return;
    }

    d->lastOpenedDirectory = url;

    // Parse image IO mime types registration to get files filter pattern.

    QString filter;
    QStringList mimeTypes = supportedImageMimeTypes(QIODevice::ReadOnly, filter);
    QString patterns      = filter.toLower();
    patterns.append(QLatin1Char(' '));
    patterns.append(filter.toUpper());

    qCDebug(DIGIKAM_SHOWFOTO_LOG) << "patterns=" << patterns;

    // Get all image files from directory.

    QDir dir(url.toLocalFile(), patterns);
    dir.setFilter(QDir::Files);

    if (!dir.exists())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QFileInfoList fileinfolist = dir.entryInfoList();

    if (fileinfolist.isEmpty())
    {
        QApplication::restoreOverrideCursor();
        return;
    }

    QFileInfoList::const_iterator fi;
    ShowfotoItemInfo iteminfo;
    QScopedPointer<DMetadata> meta(new DMetadata);

    // And open all items in image editor.

    for (fi = fileinfolist.constBegin() ; fi != fileinfolist.constEnd() ; ++fi)
    {
        iteminfo.name      = (*fi).fileName();
        iteminfo.mime      = (*fi).suffix();
        iteminfo.size      = (*fi).size();
        iteminfo.folder    = (*fi).path();
        iteminfo.url       = QUrl::fromLocalFile((*fi).filePath());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))

        iteminfo.dtime     = (*fi).birthTime();

#else

        iteminfo.dtime     = (*fi).created();

#endif

        meta->load((*fi).filePath());
        iteminfo.ctime     = meta->getItemDateTime();
        iteminfo.width     = meta->getItemDimensions().width();
        iteminfo.height    = meta->getItemDimensions().height();
        iteminfo.photoInfo = meta->getPhotographInformation();

        if (!d->infoList.contains(iteminfo))
        {
            d->infoList << iteminfo;
        }
    }

    QApplication::restoreOverrideCursor();
}

void Showfoto::slotOpenFilesfromPath(const QStringList& files, const QString& current)
{
    if (files.isEmpty())
    {
        return;
    }

    slotClearThumbBar();
    QList<QUrl> urls;

    foreach (const QString& path, files)
    {
        urls << QUrl::fromLocalFile(path);
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "slotOpenFilesfromPath:: urls list" << urls;

    openUrls(urls);

    emit signalInfoList(d->infoList);

    QUrl curl = QUrl::fromLocalFile(current);
    slotOpenUrl(d->thumbBar->showfotoItemModel()->showfotoItemInfo(curl));

    int index = d->thumbBar->thumbnailIndexForUrl(curl);
    toggleNavigation(index);

    qCDebug(DIGIKAM_GENERAL_LOG) << "slotOpenFilesfromPath:: switch to thumb index" << index;

    applySortSettings();
    d->thumbBar->setCurrentUrl(curl);
}

void Showfoto::slotAppendFilesfromPath(const QStringList& files, const QString& current)
{
    if (files.isEmpty())
    {
        return;
    }

    QList<QUrl> urls;

    foreach (const QString& file, files)
    {
        urls << QUrl::fromLocalFile(file);
    }

    slotDroppedUrls(urls, false, QUrl::fromLocalFile(current));
    applySortSettings();
}

void Showfoto::slotDroppedUrls(const QList<QUrl>& droppedUrls, bool dropped, const QUrl& current)
{
    if (droppedUrls.isEmpty())
    {
        return;
    }

    QList<QUrl> imagesUrls;
    QList<QUrl> foldersUrls;

    foreach (const QUrl& drop, droppedUrls)
    {
        if (drop.isValid())
        {
            QFileInfo info(drop.toLocalFile());
            QUrl url(QUrl::fromLocalFile(info.canonicalFilePath()));

            if (isReadableImageFile(info.absoluteFilePath()))
            {
                imagesUrls << url;
            }

            if (info.isDir())
            {
                foldersUrls << url;
            }
        }
    }

    if (!imagesUrls.isEmpty())
    {
        openUrls(imagesUrls);
    }

    if (!foldersUrls.isEmpty())
    {
        foreach (const QUrl& fUrl, foldersUrls)
        {
            openFolder(fUrl);
        }
    }

    if (!d->infoList.isEmpty())
    {
        if (!dropped && foldersUrls.isEmpty() && (imagesUrls.count() == 1))
        {
            openFolder(imagesUrls.first().adjusted(QUrl::RemoveFilename));
            d->model->clearShowfotoItemInfos();
            emit signalInfoList(d->infoList);

            slotOpenUrl(d->thumbBar->findItemByUrl(current.isValid() ? current : imagesUrls.first()));
            d->thumbBar->setCurrentUrl(current.isValid() ? current : imagesUrls.first());

            return;
        }

        d->model->clearShowfotoItemInfos();

        emit signalInfoList(d->infoList);

        slotOpenUrl(d->thumbBar->currentInfo());
    }
    else
    {
        QMessageBox::information(this, qApp->applicationName(),
                                 i18n("There is no item to process."));
        qCWarning(DIGIKAM_SHOWFOTO_LOG) << "infolist is empty..";
    }
}

void Showfoto::slotOpenUrl(const ShowfotoItemInfo& info)
{
    if (d->thumbBar->currentInfo().isNull())
    {
        return;
    }

    QString localFile;

    if (info.url.isLocalFile())
    {
        // file protocol. We do not need the network

        localFile = info.url.toLocalFile();
    }
    else
    {
        QMessageBox::critical(this, i18n("Error Loading File"),
                              i18n("Failed to load file: %1\n"
                                   "Remote file handling is not supported",
                                   info.url.fileName()));
        return;
    }

    d->currentLoadedUrl = info.url;
    d->folderView->setCurrentPath(localFile);
    m_canvas->load(localFile, m_IOFileSettings);

    //TODO : add preload here like in ImageWindow::slotLoadCurrent() ???
}

void Showfoto::slotAddedDropedItems(QDropEvent* e)
{
    QList<QUrl> list = e->mimeData()->urls();
    QList<QUrl> urls;

    foreach (const QUrl& url, list)
    {
        QFileInfo fi(url.toLocalFile());

        if (fi.exists())
        {
            urls.append(url);
        }
    }

    e->accept();

    if (!urls.isEmpty())
    {
        slotDroppedUrls(urls, true, QUrl());
    }
}

void Showfoto::slotFileWithDefaultApplication()
{
    Digikam::DFileOperations::openFilesWithDefaultApplication(QList<QUrl>() << d->thumbBar->currentUrl());
}

void Showfoto::slotOpenWith(QAction* action)
{
    openWith(d->thumbBar->currentUrl(), action);
}

} // namespace ShowFoto
