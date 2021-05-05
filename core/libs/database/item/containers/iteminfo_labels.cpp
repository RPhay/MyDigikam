/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data - Labels
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

int ItemInfo::pickLabel() const
{
    if (!m_data)
    {
        return NoPickLabel;
    }

    RETURN_IF_CACHED(pickLabel)

    int pickLabel = TagsCache::instance()->pickLabelFromTags(tagIds());

    ItemInfoWriteLocker lock;
    m_data.data()->pickLabel       = (pickLabel == -1) ? NoPickLabel : pickLabel;
    m_data.data()->pickLabelCached = true;

    return m_data->pickLabel;
}

int ItemInfo::colorLabel() const
{
    if (!m_data)
    {
        return NoColorLabel;
    }

    RETURN_IF_CACHED(colorLabel)

    int colorLabel = TagsCache::instance()->colorLabelFromTags(tagIds());

    ItemInfoWriteLocker lock;
    m_data.data()->colorLabel       = (colorLabel == -1) ? NoColorLabel : colorLabel;
    m_data.data()->colorLabelCached = true;

    return m_data->colorLabel;
}

int ItemInfo::rating() const
{
    if (!m_data)
    {
        return 0;
    }

    RETURN_IF_CACHED(rating)

    QVariantList values = CoreDbAccess().db()->getItemInformation(m_data->id, DatabaseFields::Rating);

    STORE_IN_CACHE_AND_RETURN(rating, values.first().toLongLong())
}

void ItemInfo::setPickLabel(int pickId)
{
    if (!m_data || (pickId < FirstPickLabel) || (pickId > LastPickLabel))
    {
        return;
    }

    QList<int> currentTagIds   = tagIds();
    QVector<int> pickLabelTags = TagsCache::instance()->pickLabelTags();

    // Pick Label is an exclusive tag.
    // Perform "switch" operation atomic

    {
        CoreDbAccess access;

        foreach (int tagId, currentTagIds)
        {
            if (pickLabelTags.contains(tagId))
            {
                removeTag(tagId);
            }
        }

        setTag(pickLabelTags[pickId]);
    }

    ItemInfoWriteLocker lock;
    m_data->pickLabel       = pickId;
    m_data->pickLabelCached = true;
}

void ItemInfo::setColorLabel(int colorId)
{
    if (!m_data || (colorId < FirstColorLabel) || (colorId > LastColorLabel))
    {
        return;
    }

    QList<int> currentTagIds    = tagIds();
    QVector<int> colorLabelTags = TagsCache::instance()->colorLabelTags();

    // Color Label is an exclusive tag.
    // Perform "switch" operation atomic

    {
        CoreDbAccess access;

        foreach (int tagId, currentTagIds)
        {
            if (colorLabelTags.contains(tagId))
            {
                removeTag(tagId);
            }
        }

        setTag(colorLabelTags[colorId]);
    }

    ItemInfoWriteLocker lock;
    m_data->colorLabel       = colorId;
    m_data->colorLabelCached = true;
}

void ItemInfo::setRating(int value)
{
    if (!m_data)
    {
        return;
    }

    CoreDbAccess().db()->changeItemInformation(m_data->id, QVariantList() << value, DatabaseFields::Rating);

    ItemInfoWriteLocker lock;
    m_data->rating       = value;
    m_data->ratingCached = true;
}

} // namespace Digikam
