/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data - History
 *
 * SPDX-FileCopyrightText: 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "iteminfo_p.h"

namespace Digikam
{

DImageHistory ItemInfo::imageHistory() const
{
    if (!m_data)
    {
        return DImageHistory();
    }

    ImageHistoryEntry entry = CoreDbAccess().db()->getItemHistory(m_data->id);

    return DImageHistory::fromXml(entry.history);
}

void ItemInfo::setItemHistory(const DImageHistory& history)
{
    if (!m_data)
    {
        return;
    }

    CoreDbAccess().db()->setItemHistory(m_data->id, history.toXml());
}

bool ItemInfo::hasImageHistory() const
{
    if (!m_data)
    {
        return false;
    }

    return CoreDbAccess().db()->hasImageHistory(m_data->id);
}

QString ItemInfo::uuid() const
{
    if (!m_data)
    {
        return QString();
    }

    return CoreDbAccess().db()->getImageUuid(m_data->id);
}

void ItemInfo::setUuid(const QString& uuid)
{
    if (!m_data)
    {
        return;
    }

    CoreDbAccess().db()->setImageUuid(m_data->id, uuid);
}

HistoryImageId ItemInfo::historyImageId() const
{
    if (!m_data)
    {
        return HistoryImageId();
    }

    HistoryImageId id(uuid());
    id.setCreationDate(dateTime());
    id.setFileName(name());
    id.setPathOnDisk(filePath());

    if (CoreDbAccess().db()->isUniqueHashV2())
    {
        ItemScanInfo info = CoreDbAccess().db()->getItemScanInfo(m_data->id);
        id.setUniqueHash(info.uniqueHash, info.fileSize);
    }

    return id;
}

bool ItemInfo::hasDerivedImages() const
{
    if (!m_data)
    {
        return false;
    }

    return CoreDbAccess().db()->hasImagesRelatingTo(m_data->id, DatabaseRelation::DerivedFrom);
}

bool ItemInfo::hasAncestorImages() const
{
    if (!m_data)
    {
        return false;
    }

    return CoreDbAccess().db()->hasImagesRelatedFrom(m_data->id, DatabaseRelation::DerivedFrom);
}

QList<ItemInfo> ItemInfo::derivedImages() const
{
    if (!m_data)
    {
        return QList<ItemInfo>();
    }

    return ItemInfoList(CoreDbAccess().db()->getImagesRelatingTo(m_data->id, DatabaseRelation::DerivedFrom));
}

QList<ItemInfo> ItemInfo::ancestorImages() const
{
    if (!m_data)
    {
        return QList<ItemInfo>();
    }

    return ItemInfoList(CoreDbAccess().db()->getImagesRelatedFrom(m_data->id, DatabaseRelation::DerivedFrom));
}

QList<QPair<qlonglong, qlonglong> > ItemInfo::relationCloud() const
{
    if (!m_data)
    {
        return QList<QPair<qlonglong, qlonglong> >();
    }

    return CoreDbAccess().db()->getRelationCloud(m_data->id, DatabaseRelation::DerivedFrom);
}

void ItemInfo::markDerivedFrom(const ItemInfo& ancestor)
{
    if (!m_data || ancestor.isNull())
    {
        return;
    }

    CoreDbAccess().db()->addImageRelation(m_data->id, ancestor.id(), DatabaseRelation::DerivedFrom);
}

} // namespace Digikam
