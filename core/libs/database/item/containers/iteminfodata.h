/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-05-01
 * Description : ItemInfo common data
 *
 * SPDX-FileCopyrightText: 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_INFO_DATA_H
#define DIGIKAM_ITEM_INFO_DATA_H

// Qt includes

#include <QList>
#include <QSize>
#include <QDateTime>
#include <QSharedData>
#include <QReadWriteLock>
#include <QWriteLocker>
#include <QReadLocker>

// Local includes

#include "coredburl.h"
#include "coredbalbuminfo.h"
#include "iteminfocache.h"

namespace Digikam
{

class ItemInfoStatic
{
public:

    static void create();
    static void destroy();

    static ItemInfoCache* cache();

public:

    ItemInfoCache          m_cache;
    QReadWriteLock         m_lock;

    static ItemInfoStatic* m_instance;
};

// -----------------------------------------------------------------------------------

class ItemInfoReadLocker : public QReadLocker
{
public:

    explicit ItemInfoReadLocker()
        : QReadLocker(&ItemInfoStatic::m_instance->m_lock)
    {
    }
};

// -----------------------------------------------------------------------------------

class ItemInfoWriteLocker : public QWriteLocker
{
public:

    explicit ItemInfoWriteLocker()
        : QWriteLocker(&ItemInfoStatic::m_instance->m_lock)
    {
    }
};

// -----------------------------------------------------------------------------------

class ItemInfoData : public QSharedData
{
public:

    explicit ItemInfoData();
    ~ItemInfoData();

public:

    qlonglong                                id;
    qlonglong                                currentReferenceImage;
    int                                      albumId;
    int                                      albumRootId;
    QString                                  name;

    QString                                  defaultComment;
    QString                                  defaultTitle;
    quint8                                   pickLabel;
    quint8                                   colorLabel;
    qint8                                    rating;
    DatabaseItem::Category                   category;
    QString                                  format;
    QDateTime                                creationDate;
    QDateTime                                modificationDate;
    int                                      orientation;
    qlonglong                                fileSize;
    qlonglong                                manualOrder;
    QString                                  uniqueHash;
    QSize                                    imageSize;
    QList<int>                               tagIds;
    int                                      faceCount;
    int                                      unconfirmedFaceCount;
    QMap<QString, QString>                   faceSuggestions;

    double                                   longitude;
    double                                   latitude;
    double                                   altitude;
    double                                   currentSimilarity;

    //! group leader, if the image is grouped
    qlonglong                                groupImage;

    bool                                     hasCoordinates             : 1;
    bool                                     hasAltitude                : 1;

    bool                                     defaultTitleCached         : 1;
    bool                                     defaultCommentCached       : 1;
    bool                                     pickLabelCached            : 1;
    bool                                     colorLabelCached           : 1;
    bool                                     ratingCached               : 1;
    bool                                     categoryCached             : 1;
    bool                                     formatCached               : 1;
    bool                                     creationDateCached         : 1;
    bool                                     modificationDateCached     : 1;
    bool                                     orientationCached          : 1;
    bool                                     fileSizeCached             : 1;
    bool                                     manualOrderCached          : 1;
    bool                                     uniqueHashCached           : 1;
    bool                                     imageSizeCached            : 1;
    bool                                     tagIdsCached               : 1;
    bool                                     positionsCached            : 1;
    bool                                     groupImageCached           : 1;
    bool                                     unconfirmedFaceCountCached : 1;
    bool                                     faceSuggestionsCached      : 1;
    bool                                     faceCountCached            : 1;

    bool                                     invalid                    : 1;

    // These two are initially true because we assume the data is there.
    // Once we query the data and find out it is missing, we set them to false.
    bool                                     hasVideoMetadata           : 1;
    bool                                     hasImageMetadata           : 1;


    DatabaseFields::VideoMetadataMinSizeType videoMetadataCached;
    DatabaseFields::ImageMetadataMinSizeType imageMetadataCached;

    typedef DatabaseFields::Hash<QVariant>   DatabaseFieldsHashRaw;
    DatabaseFieldsHashRaw                    databaseFieldsHashRaw;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_INFO_DATA_H
