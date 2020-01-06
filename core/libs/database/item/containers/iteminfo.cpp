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
 * Copyright (C) 2009-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
    : m_data(ItemInfoStatic::cache()->infoForId(record.imageID))
{
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

    // field is only signed 32 bit in the protocol. -1 indicates value is larger, reread

    m_data->fileSizeCached         = (m_data->fileSize != -1);
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
    : m_data(ItemInfoStatic::cache()->infoForId(ID))
{
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

QDebug operator<<(QDebug stream, const ItemInfo& info)
{
    return stream << "ItemInfo [id = " << info.id() << ", path = "
                  << info.filePath() << "]";
}

bool ItemInfo::isNull() const
{
    return !m_data;
}

qlonglong ItemInfo::id() const
{
    return (m_data ? m_data->id : -1);
}

int ItemInfo::albumId() const
{
    return (m_data ? m_data->albumId : -1);
}

int ItemInfo::albumRootId() const
{
    return (m_data ? m_data->albumRootId : -1);
}

QString ItemInfo::name() const
{
    if (!m_data)
    {
        return QString();
    }

    ItemInfoReadLocker lock;

    return m_data->name;
}

qlonglong ItemInfo::fileSize() const
{
    if (!m_data)
    {
        return 0;
    }

    RETURN_IF_CACHED(fileSize)

    QVariantList values = CoreDbAccess().db()->getImagesFields(m_data->id, DatabaseFields::FileSize);

    STORE_IN_CACHE_AND_RETURN(fileSize, values.first().toLongLong())
}

QString ItemInfo::title() const
{
    if (!m_data)
    {
        return QString();
    }

    RETURN_IF_CACHED(defaultTitle)

    QString title;
    {
        CoreDbAccess access;
        ItemComments comments(access, m_data->id);
        title = comments.defaultComment(DatabaseComment::Title);
    }

    ItemInfoWriteLocker lock;
    m_data.data()->defaultTitle       = title;
    m_data.data()->defaultTitleCached = true;

    return m_data->defaultTitle;
}

QString ItemInfo::comment() const
{
    if (!m_data)
    {
        return QString();
    }

    RETURN_IF_CACHED(defaultComment)

    QString comment;
    {
        CoreDbAccess access;
        ItemComments comments(access, m_data->id);
        comment = comments.defaultComment();
    }

    ItemInfoWriteLocker lock;
    m_data.data()->defaultComment       = comment;
    m_data.data()->defaultCommentCached = true;

    return m_data->defaultComment;
}

double ItemInfo::aspectRatio() const
{
    if (!m_data)
    {
        return 0;
    }

    RETURN_ASPECTRATIO_IF_IMAGESIZE_CACHED()

    return (double)m_data->imageSize.width() / m_data->imageSize.height();
}

qlonglong ItemInfo::manualOrder() const
{
    if (!m_data)
    {
        return 0;
    }

    RETURN_IF_CACHED(manualOrder)

    QVariantList values = CoreDbAccess().db()->getImagesFields(m_data->id, DatabaseFields::ManualOrder);

    STORE_IN_CACHE_AND_RETURN(manualOrder, values.first().toLongLong())
}

QString ItemInfo::format() const
{
    if (!m_data)
    {
        return QString();
    }

    RETURN_IF_CACHED(format)

    QVariantList values = CoreDbAccess().db()->getItemInformation(m_data->id, DatabaseFields::Format);

    STORE_IN_CACHE_AND_RETURN(format, values.first().toString())
}

DatabaseItem::Category ItemInfo::category() const
{
    if (!m_data)
    {
        return DatabaseItem::UndefinedCategory;
    }

    RETURN_IF_CACHED(category)

    QVariantList values = CoreDbAccess().db()->getImagesFields(m_data->id, DatabaseFields::Category);

    STORE_IN_CACHE_AND_RETURN(category, (DatabaseItem::Category)values.first().toInt())
}

QDateTime ItemInfo::dateTime() const
{
    if (!m_data)
    {
        return QDateTime();
    }

    RETURN_IF_CACHED(creationDate)

    QVariantList values = CoreDbAccess().db()->getItemInformation(m_data->id, DatabaseFields::CreationDate);

    STORE_IN_CACHE_AND_RETURN(creationDate, values.first().toDateTime())
}

QDateTime ItemInfo::modDateTime() const
{
    if (!m_data)
    {
        return QDateTime();
    }

    RETURN_IF_CACHED(modificationDate)

    QVariantList values = CoreDbAccess().db()->getImagesFields(m_data->id, DatabaseFields::ModificationDate);

    STORE_IN_CACHE_AND_RETURN(modificationDate, values.first().toDateTime())
}

QSize ItemInfo::dimensions() const
{
    if (!m_data)
    {
        return QSize();
    }

    RETURN_IF_CACHED(imageSize)

    QVariantList values = CoreDbAccess().db()->getItemInformation(m_data->id, DatabaseFields::Width | DatabaseFields::Height);

    ItemInfoWriteLocker lock;
    m_data.data()->imageSizeCached = true;

    if (values.size() == 2)
    {
        m_data.data()->imageSize = QSize(values.at(0).toInt(), values.at(1).toInt());
    }

    return m_data->imageSize;
}

int ItemInfo::orientation() const
{
    if (!m_data)
    {
        return 0; // ORIENTATION_UNSPECIFIED
    }

    QVariantList values = CoreDbAccess().db()->getItemInformation(m_data->id, DatabaseFields::Orientation);

    if (values.isEmpty())
    {
        return 0;
    }

    return values.first().toInt();
}

QUrl ItemInfo::fileUrl() const
{
    return QUrl::fromLocalFile(filePath());
}

QString ItemInfo::filePath() const
{
    if (!m_data)
    {
        return QString();
    }

    QString albumRoot = CollectionManager::instance()->albumRootPath(m_data->albumRootId);

    if (albumRoot.isNull())
    {
        return QString();
    }

    QString album     = ItemInfoStatic::cache()->albumRelativePath(m_data->albumId);
    ItemInfoReadLocker lock;

    if (album == QLatin1String("/"))
    {
        return (albumRoot + album + m_data->name);
    }
    else
    {
        return (albumRoot + album + QLatin1Char('/') + m_data->name);
    }
}

bool ItemInfo::isVisible() const
{
    if (!m_data)
    {
        return false;
    }

    QVariantList value = CoreDbAccess().db()->getImagesFields(m_data->id, DatabaseFields::Status);

    if (!value.isEmpty())
    {
        return (value.first().toInt() == DatabaseItem::Visible);
    }

    return false;
}

bool ItemInfo::isRemoved() const
{
    if (!m_data)
    {
        return true;
    }

    QVariantList value = CoreDbAccess().db()->getImagesFields(m_data->id, DatabaseFields::Status);

    if (!value.isEmpty())
    {
        return (value.first().toInt() == DatabaseItem::Trashed) || (value.first().toInt() == DatabaseItem::Obsolete);
    }

    return false;
}

void ItemInfo::setVisible(bool isVisible)
{
    if (!m_data)
    {
        return;
    }

    if (m_data->albumId == 0)
    {
        qCWarning(DIGIKAM_DATABASE_LOG) << "Attempt to make a Removed item visible with ItemInfo::setVisible";
        return;
    }

    CoreDbAccess().db()->setItemStatus(m_data->id, isVisible ? DatabaseItem::Visible : DatabaseItem::Hidden);
}

void ItemInfo::setManualOrder(qlonglong value)
{
    if (!m_data)
    {
        return;
    }

    CoreDbAccess().db()->setItemManualOrder(m_data->id, value);

    ItemInfoWriteLocker lock;
    m_data->manualOrder       = value;
    m_data->manualOrderCached = true;
}

void ItemInfo::setOrientation(int value)
{
    if (!m_data)
    {
        return;
    }

    CoreDbAccess().db()->changeItemInformation(m_data->id, QVariantList() << value, DatabaseFields::Orientation);
}

void ItemInfo::setName(const QString& newName)
{
    if (!m_data || newName.isEmpty())
    {
        return;
    }

    CoreDbAccess().db()->renameItem(m_data->id, newName);

    ItemInfoWriteLocker lock;
    m_data->name = newName;
    ItemInfoStatic::cache()->cacheByName(m_data);
}

void ItemInfo::setDateTime(const QDateTime& dateTime)
{
    if (!m_data || !dateTime.isValid())
    {
        return;
    }

    CoreDbAccess().db()->changeItemInformation(m_data->id, QVariantList() << dateTime, DatabaseFields::CreationDate);

    ItemInfoWriteLocker lock;
    m_data->creationDate       = dateTime;
    m_data->creationDateCached = true;
}

void ItemInfo::setModDateTime(const QDateTime& dateTime)
{
    if (!m_data || !dateTime.isValid())
    {
        return;
    }

    CoreDbAccess().db()->setItemModificationDate(m_data->id, dateTime);

    ItemInfoWriteLocker lock;
    m_data->modificationDate       = dateTime;
    m_data->modificationDateCached = true;
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

ItemInfo::DatabaseFieldsHashRaw ItemInfo::getDatabaseFieldsRaw(const DatabaseFields::Set& requestedSet) const
{
    if (!m_data || (!m_data->hasVideoMetadata && !m_data->hasImageMetadata))
    {
        return DatabaseFieldsHashRaw();
    }

    DatabaseFields::VideoMetadataMinSizeType cachedVideoMetadata;
    DatabaseFields::ImageMetadataMinSizeType cachedImageMetadata;
    ItemInfo::DatabaseFieldsHashRaw cachedHash;

    // consolidate to one ReadLocker. In particular, the shallow copy of the QHash must be done under protection

    {
        ItemInfoReadLocker lock;
        cachedVideoMetadata = m_data->videoMetadataCached;
        cachedImageMetadata = m_data->imageMetadataCached;
        cachedHash = m_data->databaseFieldsHashRaw;
    }

    if (requestedSet.hasFieldsFromVideoMetadata() && m_data->hasVideoMetadata)
    {
        const DatabaseFields::VideoMetadata requestedVideoMetadata = requestedSet.getVideoMetadata();
        const DatabaseFields::VideoMetadata missingVideoMetadata = requestedVideoMetadata & ~cachedVideoMetadata;

        if (missingVideoMetadata)
        {
            const QVariantList fieldValues = CoreDbAccess().db()->getVideoMetadata(m_data->id, missingVideoMetadata);

            ItemInfoWriteLocker lock;

            if (fieldValues.isEmpty())
            {
                m_data.data()->hasVideoMetadata    = false;
                m_data.data()->databaseFieldsHashRaw.removeAllFields(DatabaseFields::VideoMetadataAll);
                m_data.data()->videoMetadataCached = DatabaseFields::VideoMetadataNone;
            }
            else
            {
                int fieldsIndex = 0;

                for (DatabaseFields::VideoMetadataIteratorSetOnly it(missingVideoMetadata) ; !it.atEnd() ; ++it)
                {
                    const QVariant fieldValue = fieldValues.at(fieldsIndex);
                    ++fieldsIndex;

                    m_data.data()->databaseFieldsHashRaw.insertField(*it, fieldValue);
                }

                m_data.data()->videoMetadataCached |= missingVideoMetadata;
            }

            // update for return value

            cachedHash = m_data->databaseFieldsHashRaw;
        }
    }

    if (requestedSet.hasFieldsFromImageMetadata() && m_data->hasImageMetadata)
    {
        const DatabaseFields::ImageMetadata requestedImageMetadata = requestedSet.getImageMetadata();
        const DatabaseFields::ImageMetadata missingImageMetadata   = requestedImageMetadata & ~cachedImageMetadata;

        if (missingImageMetadata)
        {
            const QVariantList fieldValues = CoreDbAccess().db()->getImageMetadata(m_data->id, missingImageMetadata);

            ItemInfoWriteLocker lock;

            if (fieldValues.isEmpty())
            {
                m_data.data()->hasImageMetadata    = false;
                m_data.data()->databaseFieldsHashRaw.removeAllFields(DatabaseFields::ImageMetadataAll);
                m_data.data()->imageMetadataCached = DatabaseFields::ImageMetadataNone;
            }
            else
            {
                int fieldsIndex = 0;

                for (DatabaseFields::ImageMetadataIteratorSetOnly it(missingImageMetadata) ; !it.atEnd() ; ++it)
                {
                    const QVariant fieldValue = fieldValues.at(fieldsIndex);
                    ++fieldsIndex;

                    m_data.data()->databaseFieldsHashRaw.insertField(*it, fieldValue);
                }

                m_data.data()->imageMetadataCached |= missingImageMetadata;
            }

            cachedHash = m_data->databaseFieldsHashRaw;
        }
    }

    // We always return all fields, the caller can just retrieve the ones he needs.

    return cachedHash;
}

QVariant ItemInfo::getDatabaseFieldRaw(const DatabaseFields::Set& requestedField) const
{
    DatabaseFieldsHashRaw rawHash = getDatabaseFieldsRaw(requestedField);

    if (requestedField.hasFieldsFromImageMetadata())
    {
        const DatabaseFields::ImageMetadata requestedFieldFlag = requestedField;
        const QVariant value                                   = rawHash.value(requestedFieldFlag);

        return value;
    }

    if (requestedField.hasFieldsFromVideoMetadata())
    {
        const DatabaseFields::VideoMetadata requestedFieldFlag = requestedField;
        const QVariant value                                   = rawHash.value(requestedFieldFlag);

        return value;
    }

    return QVariant();
}

} // namespace Digikam
