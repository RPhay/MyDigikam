/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-06-17
 * Description : Find Duplicates tree-view search album item.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "findduplicatesalbumitem.h"

// Qt includes

#include <QCollator>
#include <QPainter>
#include <QIcon>

// Local includes

#include "digikam_debug.h"
#include "album.h"
#include "albummanager.h"
#include "coredbsearchxml.h"
#include "deletedialog.h"
#include "itemviewutilities.h"
#include "dio.h"

namespace Digikam
{

class Q_DECL_HIDDEN FindDuplicatesAlbumItem::Private
{

public:

    explicit Private()
      : hasThumb  (false),
        album     (nullptr),
        itemCount (0)
    {
        collator.setNumericMode(true);
        collator.setIgnorePunctuation(false);
        collator.setCaseSensitivity(Qt::CaseSensitive);
    }

    bool      hasThumb;

    SAlbum*   album;
    int       itemCount;

    QCollator collator;

    ItemInfo  refImgInfo;
};

FindDuplicatesAlbumItem::FindDuplicatesAlbumItem(QTreeWidget* const parent, SAlbum* const album)
    : QTreeWidgetItem(parent),
      d              (new Private)
{
    d->album = album;

    if (d->album)
    {
        qlonglong refImage = d->album->title().toLongLong();
        d->refImgInfo      = ItemInfo(refImage);
        setText(Column::REFERENCE_IMAGE, d->refImgInfo.name());
        setText(Column::REFERENCE_DATE,  d->refImgInfo.dateTime().toString(Qt::ISODate));

        PAlbum *const physicalAlbum = AlbumManager::instance()->findPAlbum(d->refImgInfo.albumId());
        if (physicalAlbum)
        {
            setText(Column::REFERENCE_ALBUM, physicalAlbum->prettyUrl());
        }

        calculateInfos();
    }

    setThumb(QIcon::fromTheme(QLatin1String("view-preview")).pixmap(parent->iconSize().width(),
                                                                    QIcon::Disabled), false);
}

FindDuplicatesAlbumItem::~FindDuplicatesAlbumItem()
{
    delete d;
}

bool FindDuplicatesAlbumItem::hasValidThumbnail() const
{
    return d->hasThumb;
}

QList<ItemInfo> FindDuplicatesAlbumItem::duplicatedItems()
{
    if (itemCount() <= 1)
    {
        return QList<ItemInfo>();
    }

    SearchXmlReader reader(d->album->query());
    reader.readToFirstField();

    QList<ItemInfo> toRemove;

    const QList<qlonglong>& list = reader.valueToLongLongList();
    const qlonglong refImage     = d->album->title().toLongLong();

    foreach (const qlonglong& imageId, list)
    {
        if (imageId == refImage)
        {
            continue;
        }

        toRemove.append(ItemInfo(imageId));
    }

    return toRemove;
}

void FindDuplicatesAlbumItem::calculateInfos(const QList<qlonglong>& deletedImages)
{
    if (!d->album)
    {
        return;
    }

    qlonglong refImage = d->album->title().toLongLong();
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "Calculating info for album" << refImage;
*/
    SearchXmlReader reader(d->album->query());
    reader.readToFirstField();

    // Get the defined image ids.

    const QList<qlonglong>& list = reader.valueToLongLongList();

    // Only images that are not removed/obsolete should be shown.

    QList<qlonglong> filteredList;
    double avgSim = 0.0;

    foreach (const qlonglong& imageId, list)
    {
        ItemInfo info(imageId);

        // If image is not deleted in this moment and was also not
        // removed before.

        if (!deletedImages.contains(imageId) && !info.isRemoved())
        {
            filteredList << imageId;

            if (imageId != refImage)
            {
                avgSim += info.similarityTo(refImage);
            }
        }
    }

    d->itemCount = filteredList.count();
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "New Item count:" << d->itemCount;
*/
    if (d->itemCount > 1)
    {
        avgSim /= d->itemCount - (filteredList.contains(refImage) ? 1 : 0);
    }
    else
    {
        this->setHidden(true);
    }

    setText(Column::RESULT_COUNT,   QString::number(d->itemCount));
    setText(Column::AVG_SIMILARITY, QString::number(avgSim, 'f', 2));
}

int FindDuplicatesAlbumItem::itemCount() const
{
    return d->itemCount;
}

void FindDuplicatesAlbumItem::setThumb(const QPixmap& pix, bool hasThumb)
{
    int iconSize = treeWidget()->iconSize().width();
    QPixmap pixmap(iconSize + 2, iconSize + 2);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()  / 2) - (pix.width()  / 2),
                 (pixmap.height() / 2) - (pix.height() / 2), pix);

    QIcon icon = QIcon(pixmap);

    // We make sure the preview icon stays the same regardless of the role

    icon.addPixmap(pixmap, QIcon::Selected, QIcon::On);
    icon.addPixmap(pixmap, QIcon::Selected, QIcon::Off);
    icon.addPixmap(pixmap, QIcon::Active,   QIcon::On);
    icon.addPixmap(pixmap, QIcon::Active,   QIcon::Off);
    icon.addPixmap(pixmap, QIcon::Normal,   QIcon::On);
    icon.addPixmap(pixmap, QIcon::Normal,   QIcon::Off);
    setIcon(Column::REFERENCE_IMAGE, icon);

    d->hasThumb = hasThumb;
}

SAlbum* FindDuplicatesAlbumItem::album() const
{
    return d->album;
}

QUrl FindDuplicatesAlbumItem::refUrl() const
{
    return d->refImgInfo.fileUrl();
}

bool FindDuplicatesAlbumItem::operator<(const QTreeWidgetItem& other) const
{
    int result = 0;
    int column = treeWidget()->sortColumn();

    if      (column == Column::AVG_SIMILARITY)
    {
        result = (text(column).toDouble() < other.text(column).toDouble()) ? -1 : 0;
    }
    else if (column == Column::RESULT_COUNT)
    {
        result = (text(column).toInt() < other.text(column).toInt()) ? -1 : 0;
    }
    else
    {
        result = d->collator.compare(text(column), other.text(column));
    }

    return (result < 0);
}

} // namespace Digikam
