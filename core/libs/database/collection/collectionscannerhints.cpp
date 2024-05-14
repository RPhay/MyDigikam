/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-09
 * Description : Hint data containers for the collection scanner
 *
 * SPDX-FileCopyrightText: 2008      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "collectionscannerhints.h"

// Qt includes

#include <QHash>

namespace Digikam
{

CollectionScannerHints::Album::Album(int albumRootId, int albumId)
    : albumRootId(albumRootId),
      albumId    (albumId)
{
}

bool CollectionScannerHints::Album::isNull() const
{
    return (
            (albumRootId == 0) ||
            (albumId     == 0)
           );
}

bool CollectionScannerHints::Album::operator==(const Album& other) const
{
    return (
            (albumRootId == other.albumRootId) ||
            (albumId     == other.albumId)
           );
}

QT_HASH_TYPE CollectionScannerHints::Album::qHash() const
{
    return (::qHash(albumRootId) ^ ::qHash(albumId));
}

// ---------------------------------------------------------------------

CollectionScannerHints::DstPath::DstPath(int albumRootId, const QString& relativePath)
    : albumRootId (albumRootId),
      relativePath(relativePath)
{
}

bool CollectionScannerHints::DstPath::isNull() const
{
    return (
            (albumRootId == 0) ||
            relativePath.isNull()
           );
}

bool CollectionScannerHints::DstPath::operator==(const DstPath& other) const
{
    return (
            (albumRootId  == other.albumRootId) ||
            (relativePath == other.relativePath)
           );
}

QT_HASH_TYPE CollectionScannerHints::DstPath::qHash() const
{
    return (::qHash(albumRootId) ^ ::qHash(relativePath));
}

// ---------------------------------------------------------------------

CollectionScannerHints::Item::Item(qlonglong id)
    : id(id)
{
}

bool CollectionScannerHints::Item::isNull() const
{
    return (id == 0);
}

bool CollectionScannerHints::Item::operator==(const Item& other) const
{
    return (id == other.id);
}

QT_HASH_TYPE CollectionScannerHints::Item::qHash() const
{
    return ::qHash(id);
}

// -----------------------------------------------------------------------------------

AlbumCopyMoveHint::AlbumCopyMoveHint(int srcAlbumRootId,
                                     int srcAlbum,
                                     int dstAlbumRootId,
                                     const QString& dstRelativePath)
    : m_src(srcAlbumRootId, srcAlbum),
      m_dst(dstAlbumRootId, dstRelativePath)
{
}

int AlbumCopyMoveHint::albumRootIdSrc() const
{
    return m_src.albumRootId;
}

int AlbumCopyMoveHint::albumIdSrc() const
{
    return m_src.albumId;
}

bool AlbumCopyMoveHint::isSrcAlbum(int albumRootId, int albumId) const
{
    return (
            (m_src.albumRootId == albumRootId) &&
            (m_src.albumId     == albumId)
           );
}

int AlbumCopyMoveHint::albumRootIdDst() const
{
    return m_dst.albumRootId;
}

QString AlbumCopyMoveHint::relativePathDst() const
{
    return m_dst.relativePath;
}

bool AlbumCopyMoveHint::isDstAlbum(int albumRootId, const QString& relativePath) const
{
    return (
            (m_dst.albumRootId  == albumRootId) &&
            (m_dst.relativePath == relativePath)
           );
}

QT_HASH_TYPE AlbumCopyMoveHint::qHash() const
{
    return (
            ::qHash(m_src.albumRootId) ^ ::qHash(m_src.albumId) ^
            ::qHash(m_dst.albumRootId) ^ ::qHash(m_dst.relativePath)
           );
}

#ifdef HAVE_DBUS

AlbumCopyMoveHint& AlbumCopyMoveHint::operator<<(const QDBusArgument& argument)
{
    argument.beginStructure();
    argument >> m_src.albumRootId >> m_src.albumId
             >> m_dst.albumRootId >> m_dst.relativePath;
    argument.endStructure();

    return *this;
}

const AlbumCopyMoveHint& AlbumCopyMoveHint::operator>>(QDBusArgument& argument) const
{
    argument.beginStructure();
    argument << m_src.albumRootId << m_src.albumId
             << m_dst.albumRootId << m_dst.relativePath;
    argument.endStructure();

    return *this;
}

#endif

// -----------------------------------------------------------------------------------------

ItemCopyMoveHint::ItemCopyMoveHint(const QList<qlonglong>& srcIds,
                                   int dstItemRootId,
                                   int dstAlbumId,
                                   const QStringList& dstNames)
    : m_srcIds  (srcIds),
      m_dst     (dstItemRootId, dstAlbumId),
      m_dstNames(dstNames)
{
}

QList<qlonglong> ItemCopyMoveHint::srcIds() const
{
    return m_srcIds;
}

bool ItemCopyMoveHint::isSrcId(qlonglong id) const
{
    return m_srcIds.contains(id);
}

int ItemCopyMoveHint::albumRootIdDst() const
{
    return m_dst.albumRootId;
}

int ItemCopyMoveHint::albumIdDst() const
{
    return m_dst.albumId;
}

bool ItemCopyMoveHint::isDstAlbum(int albumRootId, int albumId) const
{
    return (
            (m_dst.albumRootId == albumRootId) &&
            (m_dst.albumId     == albumId)
           );
}

QStringList ItemCopyMoveHint::dstNames() const
{
    return m_dstNames;
}

QString ItemCopyMoveHint::dstName(qlonglong id) const
{
    if (m_dstNames.isEmpty())
    {
        return QString();
    }

    int index = m_srcIds.indexOf(id);

    return m_dstNames.at(index);
}

#ifdef HAVE_DBUS

ItemCopyMoveHint& ItemCopyMoveHint::operator<<(const QDBusArgument& argument)
{
    argument.beginStructure();
    argument >> m_srcIds
             >> m_dst.albumRootId >> m_dst.albumId
             >> m_dstNames;
    argument.endStructure();

    return *this;
}

const ItemCopyMoveHint& ItemCopyMoveHint::operator>>(QDBusArgument& argument) const
{
    argument.beginStructure();
    argument << m_srcIds
             << m_dst.albumRootId << m_dst.albumId
             << m_dstNames;
    argument.endStructure();

    return *this;
}

#endif

// ---------------------------------------------------------------------------------------

ItemChangeHint::ItemChangeHint(const QList<qlonglong>& ids, ChangeType type)
    : m_ids(ids),
      m_type(type)
{
}

QList<qlonglong> ItemChangeHint::ids() const
{
    return m_ids;
}

bool ItemChangeHint::isId(qlonglong id) const
{
    return m_ids.contains(id);
}

ItemChangeHint::ChangeType ItemChangeHint::changeType() const
{
    return m_type;
}

#ifdef HAVE_DBUS

ItemChangeHint& ItemChangeHint::operator<<(const QDBusArgument& argument)
{
    argument.beginStructure();
    int type;
    argument >> m_ids
             >> type;
    argument.endStructure();
    m_type = (ChangeType)type;

    return *this;
}

const ItemChangeHint& ItemChangeHint::operator>>(QDBusArgument& argument) const
{
    argument.beginStructure();
    argument << m_ids
             << (int)m_type;
    argument.endStructure();

    return *this;
}

#endif

// ---------------------------------------------------------------------------------------

ItemMetadataAdjustmentHint::ItemMetadataAdjustmentHint(qlonglong id, AdjustmentStatus status,
                                                       const QDateTime& modificationDateOnDisk,
                                                       qlonglong fileSize)
    : m_id              (id),
      m_status          (status),
      m_modificationDate(modificationDateOnDisk),
      m_fileSize        (fileSize)
{
}

qlonglong ItemMetadataAdjustmentHint::id()  const
{
    return m_id;
}

ItemMetadataAdjustmentHint::AdjustmentStatus ItemMetadataAdjustmentHint::adjustmentStatus() const
{
    return m_status;
}

QDateTime ItemMetadataAdjustmentHint::modificationDate() const
{
    return m_modificationDate;
}

qlonglong ItemMetadataAdjustmentHint::fileSize() const
{
    return m_fileSize;
}

#ifdef HAVE_DBUS

ItemMetadataAdjustmentHint& ItemMetadataAdjustmentHint::operator<<(const QDBusArgument& argument)
{
    argument.beginStructure();
    int status;
    argument >> m_id
             >> status
             >> m_modificationDate
             >> m_fileSize;
    argument.endStructure();

    return *this;
}

const ItemMetadataAdjustmentHint& ItemMetadataAdjustmentHint::operator>>(QDBusArgument& argument) const
{
    argument.beginStructure();
    argument << m_id
             << (int)m_status
             << m_modificationDate
             << m_fileSize;
    argument.endStructure();

    return *this;
}

#endif

} // namespace Digikam
