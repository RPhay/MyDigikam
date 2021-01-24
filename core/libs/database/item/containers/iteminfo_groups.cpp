/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data - Groups
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
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

#include "iteminfo_p.h"

namespace Digikam
{

bool ItemInfo::hasGroupedImages() const
{
    return numberOfGroupedImages();
}

int ItemInfo::numberOfGroupedImages() const
{
    if (!m_data)
    {
        return 0;
    }

    return ItemInfoStatic::cache()->getImageGroupedCount(m_data->id);
}

qlonglong ItemInfo::groupImageId() const
{
    if (!m_data)
    {
        return -1;
    }

    RETURN_IF_CACHED(groupImage)

    QList<qlonglong> ids = CoreDbAccess().db()->getImagesRelatedFrom(m_data->id, DatabaseRelation::Grouped);
    // list size should be 0 or 1
    int groupImage       = ids.isEmpty() ? -1 : ids.first();

    ItemInfoWriteLocker lock;
    m_data.data()->groupImage       = groupImage;
    m_data.data()->groupImageCached = true;

    return m_data->groupImage;
}

void ItemInfoList::loadGroupImageIds() const
{
    ItemInfoList infoList;

    foreach (const ItemInfo& info, *this)
    {
        if (info.m_data && !info.m_data->groupImageCached)
        {
            infoList << info;
        }
    }

    if (infoList.isEmpty())
    {
        return;
    }

    QVector<QList<qlonglong> > allGroupIds = CoreDbAccess().db()->getImagesRelatedFrom(infoList.toImageIdList(),
                                                                                       DatabaseRelation::Grouped);

    ItemInfoWriteLocker lock;

    for (int i = 0 ; i < infoList.size() ; ++i)
    {
        const ItemInfo& info             = infoList.at(i);
        const QList<qlonglong>& groupIds = allGroupIds.at(i);

        if (!info.m_data)
        {
            continue;
        }

        info.m_data.data()->groupImage       = groupIds.isEmpty() ? -1 : groupIds.first();
        info.m_data.data()->groupImageCached = true;
    }
}

bool ItemInfo::isGrouped() const
{
    return (groupImageId() != -1);
}

ItemInfo ItemInfo::groupImage() const
{
    qlonglong id = groupImageId();

    if (id == -1)
    {
        return ItemInfo();
    }

    return ItemInfo(id);
}

QList<ItemInfo> ItemInfo::groupedImages() const
{
    if (!m_data || !hasGroupedImages())
    {
        return QList<ItemInfo>();
    }

    return ItemInfoList(CoreDbAccess().db()->getImagesRelatingTo(m_data->id, DatabaseRelation::Grouped));
}

void ItemInfo::addToGroup(const ItemInfo& givenLeader)
{
    if (!m_data || givenLeader.isNull() || (givenLeader.id() == m_data->id))
    {
        return;
    }

    // Take care: Once we start this, we cannot rely on change notifications and cache invalidation!

    CoreDbOperationGroup group;

    // Handle grouping on an already grouped image, and prevent circular grouping

    ItemInfo leader;
    QList<qlonglong> alreadySeen;
    alreadySeen << m_data->id;

    for (leader = givenLeader ; leader.isGrouped() ; )
    {
        ItemInfo nextLeader = leader.groupImage();

        // is the new leader currently grouped on this image, or do we have a circular grouping?

        if (alreadySeen.contains(nextLeader.id()))
        {
            // break loop (special case: remove b->a where we want to add a->b)

            leader.removeFromGroup();
            break;
        }
        else
        {
            alreadySeen << leader.id();
            leader = nextLeader;
        }
    }

    // Already grouped correctly?

    if (groupImageId() == leader.id())
    {
        return;
    }

    // All images grouped on this image need a new group leader

    QList<qlonglong> idsToBeGrouped  = CoreDbAccess().db()->getImagesRelatingTo(m_data->id, DatabaseRelation::Grouped);

    // and finally, this image needs to be grouped

    idsToBeGrouped << m_data->id;

    foreach (const qlonglong& ids, idsToBeGrouped)
    {
        // remove current grouping

        CoreDbAccess().db()->removeAllImageRelationsFrom(ids, DatabaseRelation::Grouped);

        // add the new grouping

        CoreDbAccess().db()->addImageRelation(ids, leader.id(), DatabaseRelation::Grouped);
    }
}

void ItemInfo::removeFromGroup()
{
    if (!m_data)
    {
        return;
    }

    if (!isGrouped())
    {
        return;
    }

    CoreDbAccess().db()->removeAllImageRelationsFrom(m_data->id, DatabaseRelation::Grouped);
}

void ItemInfo::clearGroup()
{
    if (!m_data)
    {
        return;
    }

    if (!hasGroupedImages())
    {
        return;
    }

    CoreDbAccess().db()->removeAllImageRelationsTo(m_data->id, DatabaseRelation::Grouped);
}

} // namespace Digikam
