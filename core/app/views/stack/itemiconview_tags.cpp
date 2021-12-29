/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Tag methods.
 *
 * Copyright (C) 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

void ItemIconView::setRecurseTags(bool recursive)
{
    d->iconView->imageAlbumModel()->setRecurseTags(recursive);
}

void ItemIconView::toggleTag(int tagID)
{
    ItemInfoList tagToRemove, tagToAssign;
    const ItemInfoList selectedList = selectedInfoList(ApplicationSettings::Metadata);

    foreach (const ItemInfo& info, selectedList)
    {
        if (info.tagIds().contains(tagID))
        {
            tagToRemove.append(info);
        }
        else
        {
            tagToAssign.append(info);
        }
    }

    FileActionMngr::instance()->assignTag(tagToAssign, tagID);
    FileActionMngr::instance()->removeTag(tagToRemove, tagID);
}

void ItemIconView::slotAssignPickLabel(int pickId)
{
    const ItemInfoList& itemInfos = selectedInfoList(ApplicationSettings::Metadata);
    int applyPickId               = NoPickLabel;

    foreach (const ItemInfo& info, itemInfos)
    {
        if (info.pickLabel() != pickId)
        {
            applyPickId = pickId;
            break;
        }
    }

    FileActionMngr::instance()->assignPickLabel(itemInfos, applyPickId);
}

void ItemIconView::slotAssignColorLabel(int colorId)
{
    const ItemInfoList& itemInfos = selectedInfoList(ApplicationSettings::Metadata);
    int applyColorId              = NoColorLabel;

    foreach (const ItemInfo& info, itemInfos)
    {
        if (info.colorLabel() != colorId)
        {
            applyColorId = colorId;
            break;
        }
    }

    FileActionMngr::instance()->assignColorLabel(itemInfos, applyColorId);
}

void ItemIconView::slotAssignRating(int rating)
{
    const ItemInfoList& itemInfos = selectedInfoList(ApplicationSettings::Metadata);
    int applyRating               = NoRating;

    foreach (const ItemInfo& info, itemInfos)
    {
        if (info.rating() != rating)
        {
            applyRating = rating;
            break;
        }
    }

    FileActionMngr::instance()->assignRating(itemInfos, applyRating);
}

void ItemIconView::slotAssignTag(int tagID)
{
    FileActionMngr::instance()->assignTags(selectedInfoList(ApplicationSettings::Metadata), QList<int>() << tagID);
}

void ItemIconView::slotRemoveTag(int tagID)
{
    FileActionMngr::instance()->removeTags(selectedInfoList(ApplicationSettings::Metadata), QList<int>() << tagID);

    /**
     * Implementation for Automatic Icon Removal of
     * Confirmed Tags.
     * QTimer to ensure TagRemoval is complete.
     */
    if (!FaceTags::isTheIgnoredPerson(tagID)  &&
        !FaceTags::isTheUnknownPerson(tagID)  &&
        !FaceTags::isTheUnconfirmedPerson(tagID)
       )
    {
        QTimer::singleShot(200, this, [=]()
            {
                int count = CoreDbAccess().db()->getNumberOfImagesInTagProperties(tagID,
                                                                                  ImageTagPropertyName::tagRegion());

                /**
                 * If the face just removed was the final face
                 * associated with that Tag, reset Tag Icon.
                 */
                if (count == 0)
                {
                    TAlbum* const album = AlbumManager::instance()->findTAlbum(tagID);

                    if (album && (album->iconId() != 0))
                    {
                        QString err;

                        if (!AlbumManager::instance()->updateTAlbumIcon(album, QString(),
                                                                        0, err))
                        {
                            qCDebug(DIGIKAM_GENERAL_LOG) << err ;
                        }
                    }
                }
            }
        );
    }
}


void ItemIconView::slotNewTag()
{
    QList<TAlbum*> talbums = AlbumManager::instance()->currentTAlbums();

    if (!talbums.isEmpty())
    {
        d->tagModificationHelper->slotTagNew(talbums.first());
    }
}

void ItemIconView::slotDeleteTag()
{
    QList<TAlbum*> talbums = AlbumManager::instance()->currentTAlbums();

    if (!talbums.isEmpty())
    {
        d->tagModificationHelper->slotTagDelete(talbums.first());
    }
}

void ItemIconView::slotEditTag()
{
    QList<TAlbum*> talbums = AlbumManager::instance()->currentTAlbums();

    if (!talbums.isEmpty())
    {
        d->tagModificationHelper->slotTagEdit(talbums.first());
    }
}

void ItemIconView::slotOpenTagsManager()
{
    TagsManager* const tagMngr = TagsManager::instance();
    tagMngr->show();
    tagMngr->activateWindow();
    tagMngr->raise();
}

void ItemIconView::slotAssignTag()
{
    d->rightSideBar->setActiveTab(d->rightSideBar->imageDescEditTab());
    d->rightSideBar->imageDescEditTab()->setFocusToNewTagEdit();
}

/*
void ItemIconView::slotRatingChanged(const QUrl& url, int rating)
{
    rating        = qMin(RatingMax, qMax(RatingMin, rating));
    ItemInfo info = ItemInfo::fromUrl(url);

    if (!info.isNull())
    {
        FileActionMngr::instance()->assignRating(info, rating);
    }
}

void ItemIconView::slotColorLabelChanged(const QUrl& url, int color)
{
    ItemInfo info = ItemInfo::fromUrl(url);

    if (!info.isNull())
    {
        FileActionMngr::instance()->assignColorLabel(info, color);
    }
}

void ItemIconView::slotPickLabelChanged(const QUrl& url, int pick)
{
    ItemInfo info = ItemInfo::fromUrl(url);

    if (!info.isNull())
    {
        FileActionMngr::instance()->assignPickLabel(info, pick);
    }
}

void ItemIconView::slotToggleTag(const QUrl& url, int tagID)
{
    ItemInfo info = ItemInfo::fromUrl(url);

    if (!info.isNull())
    {
        if (info.tagIds().contains(tagID))
        {
            FileActionMngr::instance()->removeTag(info, tagID);
        }
        else
        {
            FileActionMngr::instance()->assignTag(info, tagID);
        }
    }
}
*/

} // namespace Digikam
