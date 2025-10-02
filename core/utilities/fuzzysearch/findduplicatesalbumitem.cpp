/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-06-17
 * Description : Find Duplicates tree-view search album item.
 *
 * SPDX-FileCopyrightText: 2008-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "findduplicatesalbumitem.h"

// Qt includes

#include <QtConcurrentRun>
#include <QPainter>
#include <QIcon>

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "coredbsearchxml.h"
#include "deletedialog.h"
#include "itemviewutilities.h"
#include "itemsortcollator.h"
#include "dio.h"
#include "actionthreadbase.h"

namespace Digikam
{

class Q_DECL_HIDDEN FindDuplicatesAlbumItem::Private
{
public:

    Private() = default;

public:

    bool          hasThumb  = false;

    SAlbum*       album     = nullptr;
    int           itemCount = 0;

    ItemInfo      refImgInfo;

    QFuture<void> calcTask;
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

        PAlbum* const physicalAlbum = AlbumManager::instance()->findPAlbum(d->refImgInfo.albumId());

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

    for (const qlonglong& imageId : std::as_const(list))
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
    d->calcTask = QtConcurrent::run(

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

                                     &FindDuplicatesAlbumItem::calculateInfosMultithreaded, this,
                                     deletedImages

#else

                                     this, &FindDuplicatesAlbumItem::calculateInfosMultithreaded,
                                     deletedImages

#endif

                           );
}

void FindDuplicatesAlbumItem::waitForCalculate()
{
    d->calcTask.waitForFinished();
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
        result = ((text(column).toDouble() < other.text(column).toDouble()) ? -1 : 0);
    }
    else if (column == Column::RESULT_COUNT)
    {
        result = ((text(column).toInt() < other.text(column).toInt()) ? -1 : 0);
    }
    else
    {
        result = ItemSortCollator::instance()->albumCompare(text(column),
                                                            other.text(column),
                                                            Qt::CaseSensitive, true);
    }

    return (result < 0);
}

void FindDuplicatesAlbumItem::calculateInfosMultithreaded(const QList<qlonglong>& deletedImages)
{
    ActionThreadBase::setCurrentThreadName(QLatin1String(__FUNCTION__));       // To customize thread name

    if (!d->album)
    {
        return;
    }

    qlonglong refImage = d->album->title().toLongLong();
    /*
     q CDebug(DIGIKAM_GENERAL_LOG) << "Calculating info for album" << refIma*ge;
     */
    SearchXmlReader reader(d->album->query());
    reader.readToFirstField();

    // Get the defined image ids.

    const QList<qlonglong>& list = reader.valueToLongLongList();

    // Only images that are not removed/obsolete should be shown.

    QList<qlonglong> filteredList;
    double avgSim = 0.0;

    for (const qlonglong& imageId : std::as_const(list))
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
     q CDebug(DIGIKAM_GENERAL_LOG) << "New Item count:" << d->itemCount;    *
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
    setText(Column::AVG_SIMILARITY, QString::number((int)(avgSim * 100)));
}

} // namespace Digikam
