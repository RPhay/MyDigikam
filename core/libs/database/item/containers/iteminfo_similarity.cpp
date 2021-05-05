/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data - similarity
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

double ItemInfo::similarityTo(const qlonglong imageId) const
{
    return imageExtendedProperties().similarityTo(imageId);
}

double ItemInfo::currentSimilarity() const
{
    if (!m_data)
    {
        return 0;
    }

    return m_data->currentSimilarity;
}

qlonglong ItemInfo::currentReferenceImage() const
{
    if (!m_data)
    {
        return -1;
    }

    return m_data->currentReferenceImage;
}

QList<ItemInfo> ItemInfo::fromUniqueHash(const QString& uniqueHash, qlonglong fileSize)
{
    QList<ItemScanInfo> scanInfos = CoreDbAccess().db()->getIdenticalFiles(uniqueHash, fileSize);
    QList<ItemInfo> infos;

    foreach (const ItemScanInfo& scanInfo, scanInfos)
    {
        infos << ItemInfo(scanInfo.id);
    }

    return infos;
}

uint ItemInfo::hash() const
{
    if (!m_data)
    {
        return ::qHash((int)0);
    }

    return ::qHash(m_data->id);
}

QString ItemInfo::uniqueHash() const
{
    if (!m_data)
    {
        return QString();
    }

    RETURN_IF_CACHED(uniqueHash)

    QVariantList values = CoreDbAccess().db()->getImagesFields(m_data->id, DatabaseFields::UniqueHash);

    STORE_IN_CACHE_AND_RETURN(uniqueHash, values.first().toString())
}

} // namespace Digikam
