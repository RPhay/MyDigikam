/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Item methods.
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

QList<QUrl> ItemIconView::allUrls(bool grouping) const
{
    /// @todo This functions seems not to be used anywhere right now

    return allInfo(grouping).toImageUrlList();
}

QList<QUrl> ItemIconView::selectedUrls(bool grouping) const
{
    return selectedInfoList(false, grouping).toImageUrlList();
}

QList<QUrl> ItemIconView::selectedUrls(const ApplicationSettings::OperationType type) const
{
    return selectedInfoList(type).toImageUrlList();
}

void ItemIconView::slotFirstItem()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotGoToRow(0, false);
            break;

        default:
            // all other views are tied to IconView's selection model

            d->iconView->toFirstIndex();
            break;
    }
}

void ItemIconView::slotPrevItem()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotGoToRow(-1, true);
            break;

        default:
            // all other views are tied to IconView's selection model

            d->iconView->toPreviousIndex();
            break;
    }
}

void ItemIconView::slotNextItem()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotGoToRow(1, true);
            break;

        default:
            // all other views are tied to IconView's selection model

            d->iconView->toNextIndex();
            break;
    }
}

void ItemIconView::slotLastItem()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotGoToRow(-1, false);
            break;

        default:
            // all other views are tied to IconView's selection model

            d->iconView->toLastIndex();
            break;
    }
}

void ItemIconView::slotSelectItemByUrl(const QUrl& url)
{
    /// @todo This functions seems not to be used anywhere right now
    /// @todo Adapt to TableView

    d->iconView->toIndex(url);
}

void ItemIconView::slotImageSelected()
{
    // delay to slotDispatchImageSelected

    d->needDispatchSelection = true;
    d->selectionTimer->start();

    switch (viewMode())
    {
        case StackedView::TrashViewMode:
            emit signalTrashSelectionChanged(d->trashView->statusBarText());
            break;

        case StackedView::TableViewMode:
            emit signalSelectionChanged(d->tableView->numberOfSelectedItems());
            break;

        default:
            emit signalSelectionChanged(d->iconView->numberOfSelectedIndexes());
            break;
    }
}

void ItemIconView::slotDispatchImageSelected()
{
    if (viewMode() == StackedView::TrashViewMode)
    {
        d->rightSideBar->itemChanged(d->trashView->lastSelectedItemUrl());
        return;
    }

    if (d->needDispatchSelection)
    {
        // the list of ItemInfos of currently selected items, currentItem first
        ApplicationSettings::ApplyToEntireGroup applyAll =
            ApplicationSettings::instance()->getGroupingOperateOnAll(ApplicationSettings::Metadata);
        const ItemInfoList list      = selectedInfoList(true, (applyAll == ApplicationSettings::Yes));
        const ItemInfoList allImages = allInfo(true);

        if (list.isEmpty())
        {
            d->stackedview->setPreviewItem();
            emit signalImageSelected(list, allImages);
            emit signalNoCurrentItem();
        }
        else
        {
            d->rightSideBar->itemChanged(list, allImages);

            ItemInfo previousInfo;
            ItemInfo nextInfo;

            if (viewMode() == StackedView::TableViewMode)
            {
                previousInfo = d->tableView->previousInfo();
                nextInfo     = d->tableView->nextInfo();
            }
            else
            {
                previousInfo = d->iconView->previousInfo(list.first());
                nextInfo     = d->iconView->nextInfo(list.first());
            }

            if (
                (viewMode() != StackedView::IconViewMode)  &&
                (viewMode() != StackedView::MapWidgetMode) &&
                (viewMode() != StackedView::TableViewMode)
               )
            {
                d->stackedview->setPreviewItem(list.first(), previousInfo, nextInfo);
            }

            emit signalImageSelected(list, allImages);
        }

        d->needDispatchSelection = false;
    }
}

void ItemIconView::slotImageWriteMetadata()
{
    const ItemInfoList selected      = selectedInfoList(ApplicationSettings::Metadata);
    MetadataSynchronizer* const tool = new MetadataSynchronizer(selected, MetadataSynchronizer::WriteFromDatabaseToFile);
    tool->start();
}

void ItemIconView::slotImageReadMetadata()
{
    const ItemInfoList selected      = selectedInfoList(ApplicationSettings::Metadata);
    MetadataSynchronizer* const tool = new MetadataSynchronizer(selected, MetadataSynchronizer::ReadFromFileToDatabase);
    tool->setUseMultiCoreCPU(false);
    tool->start();
}

void ItemIconView::slotRefreshImagePreview()
{
    if (viewMode() == StackedView::PreviewImageMode)
    {
        d->stackedview->imagePreviewView()->reload();
    }
}

void ItemIconView::slotImageRename()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->rename();
            break;

        default:
            d->iconView->rename();
            break;
    }
}

void ItemIconView::slotImageDelete()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotDeleteSelected(ItemViewUtilities::DeleteUseTrash);
            break;

        default:
            d->iconView->deleteSelected(ItemViewUtilities::DeleteUseTrash);
            break;
    }
}

void ItemIconView::slotImageDeletePermanently()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotDeleteSelected(ItemViewUtilities::DeletePermanently);
            break;

        default:
            d->iconView->deleteSelected(ItemViewUtilities::DeletePermanently);
            break;
    }
}

void ItemIconView::slotImageDeletePermanentlyDirectly()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotDeleteSelectedWithoutConfirmation(ItemViewUtilities::DeletePermanently);
            break;

        default:
            d->iconView->deleteSelectedDirectly(ItemViewUtilities::DeletePermanently);
            break;
    }
}

void ItemIconView::slotImageTrashDirectly()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotDeleteSelectedWithoutConfirmation(ItemViewUtilities::DeleteUseTrash);
            break;

        default:
            d->iconView->deleteSelectedDirectly(ItemViewUtilities::DeleteUseTrash);
            break;
    }
}

void ItemIconView::slotSelectAll()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->selectAll();
            break;

        default:
            d->iconView->selectAll();
            break;
    }
}

void ItemIconView::slotSelectNone()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->clearSelection();
            break;

        default:
            d->iconView->clearSelection();
            break;
    }
}

void ItemIconView::slotSelectInvert()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->invertSelection();
            break;

        default:
            d->iconView->invertSelection();
            break;
    }
}

void ItemIconView::slotSortImages(int sortRole)
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    settings->setImageSortOrder(sortRole);
    d->iconView->imageFilterModel()->setSortRole((ItemSortSettings::SortRole) sortRole);
    settings->emitSetupChanged();
}

void ItemIconView::slotSortImagesOrder(int order)
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    settings->setImageSorting(order);
    d->iconView->imageFilterModel()->setSortOrder((ItemSortSettings::SortOrder) order);
    settings->emitSetupChanged();
}

void ItemIconView::slotSeparateImages(int categoryMode)
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    settings->setImageSeparationMode(categoryMode);
    d->iconView->imageFilterModel()->setCategorizationMode((ItemSortSettings::CategorizationMode) categoryMode);
}

void ItemIconView::slotImageSeparationSortOrder(int order)
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    settings->setImageSeparationSortOrder(order);
    d->iconView->imageFilterModel()->setCategorizationSortOrder((ItemSortSettings::SortOrder) order);
}

void ItemIconView::slotImagePaste()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotPaste();
            break;

        default:
            d->iconView->paste();
            break;
    }
}

void ItemIconView::slotImageChangeFailed(const QString& message, const QStringList& fileNames)
{
    if (fileNames.isEmpty())
    {
        return;
    }

    DMessageBox::showInformationList(QMessageBox::Critical,
                                     qApp->activeWindow(),
                                     qApp->applicationName(),
                                     message,
                                     fileNames);
}

bool ItemIconView::hasCurrentItem() const
{
    return !currentInfo().isNull();
}

void ItemIconView::slotFocusAndNextImage()
{
    // slot is called on pressing "return" a second time after assigning a tag

    d->stackedview->currentWidget()->setFocus();

    // select next image, since the user is probably done tagging the current image

    slotNextItem();
}

void ItemIconView::slotImageExifOrientation(int orientation)
{
    FileActionMngr::instance()->setExifOrientation(
                selectedInfoList(ApplicationSettings::Metadata), orientation);
}

void ItemIconView::imageTransform(MetaEngineRotation::TransformationAction transform)
{
    FileActionMngr::instance()->transform(
                selectedInfoList(ApplicationSettings::Metadata), transform);
}

ItemInfo ItemIconView::currentInfo() const
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            return d->tableView->currentInfo();

#ifdef HAVE_MARBLE

        case StackedView::MapWidgetMode:
            return d->mapView->currentItemInfo();

#endif // HAVE_MARBLE

        case StackedView::MediaPlayerMode:
        case StackedView::PreviewImageMode:
        case StackedView::IconViewMode:
            // all of these modes use the same selection model and data as the IconViewMode

            return d->iconView->currentInfo();

        default:
            return ItemInfo();
    }
}

ItemInfoList ItemIconView::selectedInfoList(const bool currentFirst,
                                            const bool grouping) const
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:

            if (currentFirst)
            {
                return d->tableView->selectedItemInfosCurrentFirst(grouping);
            }

            return d->tableView->selectedItemInfos(grouping);

        case StackedView::PreviewImageMode:
        case StackedView::MediaPlayerMode:
        case StackedView::MapWidgetMode:
        case StackedView::IconViewMode:

            // all of these modes use the same selection model and data as the IconViewMode

            if (currentFirst)
            {
                return d->iconView->selectedItemInfosCurrentFirst(grouping);
            }

            return d->iconView->selectedItemInfos(grouping);

        default:

            return ItemInfoList();
    }
}

ItemInfoList ItemIconView::selectedInfoList(const ApplicationSettings::OperationType type,
                                            const bool currentFirst) const
{
    return selectedInfoList(currentFirst, selectedNeedGroupResolving(type));
}

ItemInfoList ItemIconView::allInfo(const bool grouping) const
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            return d->tableView->allItemInfos(grouping);

        case StackedView::MapWidgetMode:
        case StackedView::PreviewImageMode:
        case StackedView::MediaPlayerMode:
        case StackedView::IconViewMode:
            // all of these modes use the same selection model and data as the IconViewMode

            return d->iconView->allItemInfos(grouping);

        default:
            return ItemInfoList();
    }
}

ItemInfoList ItemIconView::allInfo(const ApplicationSettings::OperationType type) const
{
    return allInfo(allNeedGroupResolving(type));
}

int ItemIconView::itemCount() const
{
    return d->iconView->imageModel()->itemCount();
}

QUrl ItemIconView::currentUrl() const
{
    const ItemInfo cInfo = currentInfo();

    return cInfo.fileUrl();
}

void ItemIconView::slotSetCurrentUrlWhenAvailable(const QUrl& url)
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotSetCurrentUrlWhenAvailable(url);
            break;

        default:
            d->iconView->setCurrentUrlWhenAvailable(url);
            break;
    }
}

void ItemIconView::slotSetCurrentWhenAvailable(const qlonglong id)
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotSetCurrentWhenAvailable(id);
            break;

        default:
            d->iconView->setCurrentWhenAvailable(id);
            break;
    }
}

void ItemIconView::slotAwayFromSelection()
{
    switch (viewMode())
    {
        case StackedView::TableViewMode:
            d->tableView->slotAwayFromSelection();
            break;

        default:
            d->iconView->awayFromSelection();
            break;
    }
}

void ItemIconView::slotCopySelectionTo()
{
    d->utilities->copyItemsToExternalFolder(selectedInfoList(false, true));
}

} // namespace Digikam
