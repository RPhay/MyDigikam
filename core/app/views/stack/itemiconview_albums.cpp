/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Album methods.
 *
 * Copyright (C) 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2011-2013 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
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

#include "itemiconview_p.h"

namespace Digikam
{

void ItemIconView::slotAllAlbumsLoaded()
{
    disconnect(d->albumManager, SIGNAL(signalAllAlbumsLoaded()),
               this, SLOT(slotAllAlbumsLoaded()));

    loadViewState();
    d->leftSideBar->loadState();
    d->rightSideBar->loadState();
    d->rightSideBar->populateTags();

    // now that all albums have been loaded, activate the albumHistory

    d->useAlbumHistory = true;
    Album* const album = d->albumManager->findAlbum(d->initialAlbumID);
    d->albumManager->setCurrentAlbums(QList<Album*>() << album);
}

void ItemIconView::slotSortAlbums(int role)
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    settings->setAlbumSortRole((ApplicationSettings::AlbumSortRole) role);
    settings->saveSettings();

    // A dummy way to force the tree view to resort if the album sort role changed

    PAlbum* const albumBeforeSorting = d->albumFolderSideBar->currentAlbum();
    settings->setAlbumSortChanged(true);
    d->albumFolderSideBar->doSaveState();
    d->albumFolderSideBar->doLoadState();
    d->albumFolderSideBar->doSaveState();
    d->albumFolderSideBar->doLoadState();
    settings->setAlbumSortChanged(false);

    if (d->leftSideBar->getActiveTab() == d->albumFolderSideBar)
    {
        d->albumFolderSideBar->setCurrentAlbum(albumBeforeSorting);
    }
}

void ItemIconView::slotNewAlbum()
{
    // TODO use the selection model of the view instead

    d->albumModificationHelper->slotAlbumNew(d->albumFolderSideBar->currentAlbum());
}

void ItemIconView::slotDeleteAlbum()
{
    d->albumModificationHelper->slotAlbumDelete(d->albumFolderSideBar->currentAlbum());
}

void ItemIconView::slotRenameAlbum()
{
    d->albumModificationHelper->slotAlbumRename(d->albumFolderSideBar->currentAlbum());
}

void ItemIconView::slotAlbumsCleared()
{
    emit signalAlbumSelected(nullptr);
}

void ItemIconView::slotAlbumHistoryBack(int steps)
{
    QList<Album*> albums;
    QWidget* widget = nullptr;

    d->albumHistory->back(albums, &widget, steps);

    changeAlbumFromHistory(albums, widget);
}

void ItemIconView::slotAlbumHistoryForward(int steps)
{
    QList<Album*> albums;
    QWidget* widget = nullptr;

    d->albumHistory->forward(albums, &widget, steps);

    changeAlbumFromHistory(albums , widget);
}

// TODO update, use SideBarWidget instead of QWidget

void ItemIconView::changeAlbumFromHistory(const QList<Album*>& album, QWidget* const widget)
{
    if (!(album.isEmpty()) && widget)
    {
        // TODO update, temporary casting until signature is changed

        SidebarWidget* const sideBarWidget = dynamic_cast<SidebarWidget*>(widget);

        if (sideBarWidget)
        {
            sideBarWidget->changeAlbumFromHistory(album);
            slotLeftSideBarActivate(sideBarWidget);

            if (sideBarWidget == d->labelsSideBar)
            {
                d->labelsSearchHandler->restoreSelectionFromHistory(d->albumHistory->neededLabels());
            }
        }

        d->parent->enableAlbumBackwardHistory(d->useAlbumHistory && !d->albumHistory->isBackwardEmpty());
        d->parent->enableAlbumForwardHistory(d->useAlbumHistory && !d->albumHistory->isForwardEmpty());
    }
}


void ItemIconView::slotSelectAlbum(const QUrl& url)
{
    PAlbum* const album = d->albumManager->findPAlbum(url);

    if (!album)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Unable to find album for " << url;
        return;
    }

    slotLeftSideBarActivate(d->albumFolderSideBar);
    d->albumFolderSideBar->setCurrentAlbum(album);
}

void ItemIconView::slotAlbumSelected(const QList<Album*>& albums)
{
    emit signalNoCurrentItem();
    emit signalAlbumSelected(nullptr);

    if (albums.isEmpty() || !albums.first())
    {
        d->iconView->openAlbum(QList<Album*>());

#ifdef HAVE_MARBLE

        d->mapView->openAlbum(nullptr);

#endif // HAVE_MARBLE

        slotTogglePreviewMode(ItemInfo());
        return;
    }

    Album* const album = albums.first();
    emit signalAlbumSelected(album);

    if (d->useAlbumHistory && !d->labelsSearchHandler->isRestoringSelectionFromHistory())
    {
        if (!(d->leftSideBar->getActiveTab() == d->labelsSideBar))
        {
            d->albumHistory->addAlbums(albums, d->leftSideBar->getActiveTab());
        }
        else
        {
            if (albums.first()->isUsedByLabelsTree())
            {
                d->albumHistory->addAlbums(albums, d->leftSideBar->getActiveTab(), d->labelsSideBar->selectedLabels());
            }
        }
    }

    d->parent->enableAlbumBackwardHistory(d->useAlbumHistory && !d->albumHistory->isBackwardEmpty());
    d->parent->enableAlbumForwardHistory(d->useAlbumHistory && !d->albumHistory->isForwardEmpty());

    d->iconView->openAlbum(albums);

    if (album->isRoot())
    {
        d->stackedview->setViewMode(StackedView::WelcomePageMode);
    }
    else if (album->isTrashAlbum())
    {
        PAlbum* const palbum = d->albumManager->findPAlbum(album->parent()->id());

        if (palbum)
        {
            slotTogglePreviewMode(ItemInfo());
            QUrl url = palbum->fileUrl().adjusted(QUrl::StripTrailingSlash);
            d->trashView->model()->loadItemsForCollection(url.toLocalFile());
            d->stackedview->setViewMode(StackedView::TrashViewMode);
            d->filterWidget->setEnabled(false);
        }
    }
    else
    {
        switch (viewMode())
        {
            case StackedView::PreviewImageMode:
            case StackedView::MediaPlayerMode:
            case StackedView::WelcomePageMode:
            case StackedView::TrashViewMode:
                slotTogglePreviewMode(ItemInfo());
                break;

            default:
                break;
        }

        d->filterWidget->setEnabled(true);
    }
}

void ItemIconView::slotGotoAlbumAndItem(const ItemInfo& imageInfo)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "going to " << imageInfo;

    emit signalNoCurrentItem();

    PAlbum* const album = AlbumManager::instance()->findPAlbum(imageInfo.albumId());

    d->albumFolderSideBar->setCurrentAlbum(album);
    slotLeftSideBarActivate(d->albumFolderSideBar);

    // Set the activate item url to find in the Album View after
    // all items have be reloaded.

    slotSetCurrentWhenAvailable(imageInfo.id());

    // And finally toggle album manager to handle album history and
    // reload all items.

    d->albumManager->setCurrentAlbums(QList<Album*>() << album);
}

void ItemIconView::slotGotoDateAndItem(const ItemInfo& imageInfo)
{
    QDate date = imageInfo.dateTime().date();

    emit signalNoCurrentItem();

    // Change to Date Album view.
    // Note, that this also opens the side bar if it is closed; this is
    // considered as a feature, because it highlights that the view was changed.

    slotLeftSideBarActivate(d->dateViewSideBar);

    // Set the activate item url to find in the Album View after
    // all items have be reloaded.

    slotSetCurrentWhenAvailable(imageInfo.id());

    // Change the year and month of the iconItem (day is unused).

    d->dateViewSideBar->gotoDate(date);
}

void ItemIconView::slotGotoTagAndItem(int tagID)
{
    // FIXME: Arnd: don't know yet how to get the iconItem passed through ...
    //  then we would know how to use the following ...
    //  KURL url(iconItem->imageInfo()->kurl());
    //  url.cleanPath();

    emit signalNoCurrentItem();

    // Change to Tag Folder view.
    // Note, that this also opens the side bar if it is closed; this is
    // considered as a feature, because it highlights that the view was changed.

    slotLeftSideBarActivate(d->tagViewSideBar);

    // Set the current tag in the tag folder view.
    // TODO this slot should use a TAlbum pointer directly

    TAlbum* const tag = AlbumManager::instance()->findTAlbum(tagID);

    if (tag)
    {
        d->tagViewSideBar->setCurrentAlbum(tag);
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Could not find a tag album for tag id " << tagID;
    }

    // Set the activate item url to find in the Tag View after
    // all items have be reloaded.
    // FIXME: see above
    // d->iconView->setAlbumItemToFind(url);
}

void ItemIconView::slotAlbumOpenInFileManager()
{
    Album* const album = d->albumManager->currentAlbums().constFirst();

    if (!album || (album->type() != Album::PHYSICAL))
    {
        return;
    }

    if (album->isRoot())
    {
        QMessageBox::critical(this, qApp->applicationName(),
                              i18n("Cannot open the root album. It is not a physical location."));
        return;
    }

    QList<QUrl> urls = selectedInfoList(true, true).toImageUrlList();

    if (!urls.isEmpty())
    {
        DFileOperations::openInFileManager(urls);
    }
    else
    {
        PAlbum* const palbum = dynamic_cast<PAlbum*>(album);

        if (palbum)
        {
            QUrl url(QUrl::fromLocalFile(palbum->folderPath()));
            DFileOperations::openInFileManager(QList<QUrl>() << url);
        }
    }
}

void ItemIconView::slotAlbumRefreshComplete()
{
    // force reload. Should normally not be necessary, but we may have bugs

    qlonglong currentId = currentInfo().id();
    d->iconView->imageAlbumModel()->refresh();

    if (currentId != -1)
    {
        slotSetCurrentWhenAvailable(currentId);
    }
}

void ItemIconView::slotAlbumPropsEdit()
{
    d->albumModificationHelper->slotAlbumEdit(d->albumManager->currentPAlbum());
}

void ItemIconView::slotAlbumWriteMetadata()
{
    Album* const album = d->albumManager->currentAlbums().constFirst();

    if (!album)
    {
        return;
    }

    MetadataSynchronizer* const tool = new MetadataSynchronizer(AlbumList() << album, MetadataSynchronizer::WriteFromDatabaseToFile);
    tool->start();
}

void ItemIconView::slotAlbumReadMetadata()
{
    Album* const album = d->albumManager->currentAlbums().constFirst();

    if (!album)
    {
        return;
    }

    MetadataSynchronizer* const tool = new MetadataSynchronizer(AlbumList() << album, MetadataSynchronizer::ReadFromFileToDatabase);
    tool->setUseMultiCoreCPU(false);
    tool->start();
}

void ItemIconView::setRecurseAlbums(bool recursive)
{
    d->iconView->imageAlbumModel()->setRecurseAlbums(recursive);
}

Album* ItemIconView::currentAlbum() const
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            return d->tableView->currentAlbum();

        case StackedView::PreviewImageMode:
        case StackedView::MediaPlayerMode:
        case StackedView::MapWidgetMode:
        case StackedView::IconViewMode:
            // all of these modes use the same selection model and data as the IconViewMode

            return d->iconView->currentAlbum();

        default:
            return nullptr;
    }
}

void ItemIconView::slotSetAsAlbumThumbnail(const ItemInfo& info)
{
    d->utilities->setAsAlbumThumbnail(currentAlbum(), info);
}

void ItemIconView::slotMoveSelectionToAlbum()
{
    d->utilities->createNewAlbumForInfos(selectedInfoList(false, true),
                                         currentAlbum());
}

void ItemIconView::slotRefresh()
{
    switch (viewMode())
    {
        case StackedView::PreviewImageMode:
            d->stackedview->imagePreviewView()->reload();
            break;

#ifdef HAVE_MEDIAPLAYER

        case StackedView::MediaPlayerMode:
            d->stackedview->mediaPlayerView()->reload();
            break;

#endif //HAVE_MEDIAPLAYER

        default:
            Album* const album = currentAlbum();

            if (!album)
            {
                return;
            }

            if (qApp->keyboardModifiers() != Qt::ControlModifier)
            {
                // force reloading of thumbnails

                LoadingCacheInterface::cleanThumbnailCache();

                ThumbsGenerator* const tool = new ThumbsGenerator(true, album->id());
                tool->start();
            }

            // if physical album, schedule a collection scan of current album's path

            if (album->type() == Album::PHYSICAL)
            {
                NewItemsFinder* const tool2 = new NewItemsFinder(NewItemsFinder::ScheduleCollectionScan,
                                                                 QStringList() << static_cast<PAlbum*>(album)->folderPath());

                connect(tool2, SIGNAL(signalComplete()),
                        this, SLOT(slotAlbumRefreshComplete()));

                tool2->start();
            }

            break;
    }
}

void ItemIconView::clearHistory()
{
    d->albumHistory->clearHistory();
    d->parent->enableAlbumBackwardHistory(false);
    d->parent->enableAlbumForwardHistory(false);
}

void ItemIconView::getBackwardHistory(QStringList& titles)
{
    d->albumHistory->getBackwardHistory(titles);

    for (int i = 0 ; i < titles.size() ; ++i)
    {
        titles[i] = d->userPresentableAlbumTitle(titles.at(i));
    }
}

void ItemIconView::getForwardHistory(QStringList& titles)
{
    d->albumHistory->getForwardHistory(titles);

    for (int i = 0 ; i < titles.size() ; ++i)
    {
        titles[i] = d->userPresentableAlbumTitle(titles.at(i));
    }
}

} // namespace Digikam
