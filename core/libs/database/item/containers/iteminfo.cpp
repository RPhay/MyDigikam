/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data
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

ItemInfo::ItemInfo()
    : m_data(nullptr)
{
}

ItemInfo::ItemInfo(const ItemListerRecord& record)
{
    // cppcheck-suppress useInitializationList
    m_data                         = ItemInfoStatic::cache()->infoForId(record.imageID);

    ItemInfoWriteLocker lock;
    bool newlyCreated              = (m_data->albumId == -1);

    m_data->albumId                = record.albumID;
    m_data->albumRootId            = record.albumRootID;
    m_data->name                   = record.name;

    m_data->rating                 = record.rating;
    m_data->category               = record.category;
    m_data->format                 = record.format;
    m_data->creationDate           = record.creationDate;
    m_data->modificationDate       = record.modificationDate;
    m_data->fileSize               = record.fileSize;
    m_data->imageSize              = record.imageSize;
    m_data->currentSimilarity      = record.currentSimilarity;
    m_data->currentReferenceImage  = record.currentFuzzySearchReferenceImage;

    m_data->ratingCached           = true;
    m_data->categoryCached         = true;
    m_data->formatCached           = true;
    m_data->creationDateCached     = true;
    m_data->modificationDateCached = true;
    m_data->fileSizeCached         = true;
    m_data->imageSizeCached        = true;
    m_data->videoMetadataCached    = DatabaseFields::VideoMetadataNone;
    m_data->imageMetadataCached    = DatabaseFields::ImageMetadataNone;
    m_data->hasVideoMetadata       = true;
    m_data->hasImageMetadata       = true;
    m_data->databaseFieldsHashRaw.clear();

    if (newlyCreated)
    {
        ItemInfoStatic::cache()->cacheByName(m_data);
    }
}

ItemInfo::ItemInfo(qlonglong ID)
{
    // cppcheck-suppress useInitializationList
    m_data = ItemInfoStatic::cache()->infoForId(ID);

    // is this a newly created structure, need to populate?

    if (m_data->albumId == -1)
    {
        // retrieve immutable values now, the rest on demand

        ItemShortInfo info = CoreDbAccess().db()->getItemShortInfo(ID);

        if (info.id)
        {
            ItemInfoWriteLocker lock;
            m_data->albumId     = info.albumID;
            m_data->albumRootId = info.albumRootID;
            m_data->name        = info.itemName;
            ItemInfoStatic::cache()->cacheByName(m_data);
        }
        else
        {
            // invalid image id

            ItemInfoStatic::cache()->dropInfo(m_data);
            m_data.reset();
        }
    }
}

ItemInfo::~ItemInfo()
{
    ItemInfoStatic::cache()->dropInfo(m_data);
    m_data.reset();
}

ItemInfo::ItemInfo(const ItemInfo& info)
    : m_data(info.m_data)
{
}

ItemInfo& ItemInfo::operator=(const ItemInfo& info)
{
    if (m_data == info.m_data)
    {
        return *this;
    }

    ItemInfoStatic::cache()->dropInfo(m_data);
    m_data = info.m_data;

    return *this;
}

bool ItemInfo::operator==(const ItemInfo& info) const
{
    if (m_data && info.m_data)
    {
        // not null, compare id
        return (m_data->id == info.m_data->id);
    }
    else
    {
        // both null?
        return (m_data == info.m_data);
    }
}

bool ItemInfo::operator!=(const ItemInfo& info) const
{
    return (!operator==(info));
}

bool ItemInfo::operator<(const ItemInfo& info) const
{
    if (m_data)
    {
        if (info.m_data)
        {
            // both not null, sort by id
            return (m_data->id < info.m_data->id);
        }
        else
        {
            // only other is null, this is greater than
            return false;
        }
    }
    else
    {
        // this is less than if the other is not null
        return info.m_data;
    }
}

ItemInfo ItemInfo::fromUrl(const QUrl& url)
{
    return fromLocalFile(url.toLocalFile());
}

ItemInfo ItemInfo::fromLocalFile(const QString& path)
{
    CollectionLocation location = CollectionManager::instance()->locationForPath(path);

    if (location.isNull())
    {
        qCWarning(DIGIKAM_DATABASE_LOG) << "No location could be retrieved for" << path;
        return ItemInfo();
    }

    QUrl url      = QUrl::fromLocalFile(path);
    QString album = CollectionManager::instance()->album(url.adjusted(QUrl::RemoveFilename |
                                                                      QUrl::StripTrailingSlash).toLocalFile());
    QString name  = url.fileName();

    return fromLocationAlbumAndName(location.id(), album, name);
}

ItemInfo ItemInfo::fromLocationAlbumAndName(int locationId, const QString& album, const QString& name)
{
    if (!locationId || album.isEmpty() || name.isEmpty())
    {
        return ItemInfo();
    }

    ItemInfo info;

    // Cached ?
    info.m_data = ItemInfoStatic::cache()->infoForPath(locationId, album, name);

    if (!info.m_data)
    {

        ItemShortInfo shortInfo  = CoreDbAccess().db()->getItemShortInfo(locationId, album, name);

        if (!shortInfo.id)
        {
            qCWarning(DIGIKAM_DATABASE_LOG) << "No itemShortInfo could be retrieved from the database for image" << name;
            info.m_data.reset();
            return info;
        }

        info.m_data              = ItemInfoStatic::cache()->infoForId(shortInfo.id);

        ItemInfoWriteLocker lock;
        info.m_data->albumId     = shortInfo.albumID;
        info.m_data->albumRootId = shortInfo.albumRootID;
        info.m_data->name        = shortInfo.itemName;

        ItemInfoStatic::cache()->cacheByName(info.m_data);
    }

    return info;
}

ItemInfo ItemInfo::copyItem(int dstAlbumID, const QString& dstFileName)
{
    if (!m_data)
    {
        return ItemInfo();
    }

    {
        ItemInfoReadLocker lock;

        if ((dstAlbumID == m_data->albumId) && (dstFileName == m_data->name))
        {
            return (*this);
        }
    }

    int id = CoreDbAccess().db()->copyItem(m_data->albumId, m_data->name, dstAlbumID, dstFileName);

    if (id == -1)
    {
        return ItemInfo();
    }

    return ItemInfo(id);
}

bool ItemInfo::isLocationAvailable() const
{
    if (!m_data)
    {
        return false;
    }

    return CollectionManager::instance()->locationForAlbumRootId(m_data->albumRootId).isAvailable();
}

QDebug operator<<(QDebug stream, const ItemInfo& info)
{
    return stream << "ItemInfo [id = " << info.id() << ", path = "
                  << info.filePath() << "]";
}

} // namespace Digikam
