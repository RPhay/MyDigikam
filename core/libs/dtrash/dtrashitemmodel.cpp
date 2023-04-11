/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-09
 * Description : DTrash item info model
 *
 * SPDX-FileCopyrightText: 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dtrashitemmodel.h"

// Qt includes

#include <QIcon>
#include <QTimer>
#include <QPixmap>
#include <QMimeType>
#include <QApplication>
#include <QMimeDatabase>
#include <QPersistentModelIndex>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "thumbnailsize.h"
#include "iojobsmanager.h"

namespace Digikam
{

class Q_DECL_HIDDEN DTrashItemModel::Private
{

public:

    explicit Private()
      : thumbSize         (ThumbnailSize::Large),
        sortColumn        (DTrashTimeStamp),
        sortOrder         (Qt::DescendingOrder),
        itemsLoadingThread(nullptr),
        thumbnailThread   (nullptr)
    {
    }

public:

    int                  thumbSize;
    int                  sortColumn;

    Qt::SortOrder        sortOrder;

    IOJobsThread*        itemsLoadingThread;
    ThumbnailLoadThread* thumbnailThread;

    QString              trashAlbumPath;

    QStringList          failedThumbnails;
    QStringList          collectionThumbs;
    DTrashItemInfoList   data;
};

DTrashItemModel::DTrashItemModel(QObject* const parent)
    : QAbstractTableModel(parent),
      d                  (new Private)
{
    qRegisterMetaType<DTrashItemInfo>("DTrashItemInfo");
    d->thumbnailThread = new ThumbnailLoadThread;
    d->thumbnailThread->setSendSurrogatePixmap(false);

    connect(d->thumbnailThread, SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
            this, SLOT(refreshThumbnails(LoadingDescription,QPixmap)));
}

DTrashItemModel::~DTrashItemModel()
{
    delete d->thumbnailThread;
    delete d;
}

int DTrashItemModel::rowCount(const QModelIndex&) const
{
    return d->data.count();
}

int DTrashItemModel::columnCount(const QModelIndex&) const
{
    return DTrashNumCol;
}

QVariant DTrashItemModel::data(const QModelIndex& index, int role) const
{
    if (
        (role != Qt::DisplayRole)       &&
        (role != Qt::DecorationRole)    &&
        (role != Qt::TextAlignmentRole) &&
        (role != Qt::ToolTipRole)
       )
    {
        return QVariant();
    }

    const DTrashItemInfo& item = d->data[index.row()];

    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    if ((role == Qt::DecorationRole) && (index.column() == DTrashThumb))
    {
        QPixmap pix;
        QString thumbPath;

        if      (!d->failedThumbnails.contains(item.collectionPath))
        {
            d->collectionThumbs << item.collectionPath;
            thumbPath = item.collectionPath;
        }
        else if (!d->failedThumbnails.contains(item.trashPath))
        {
            thumbPath = item.trashPath;
        }

        if (thumbPath.isEmpty() || pixmapForItem(thumbPath, pix))
        {
            if (pix.isNull())
            {
                QMimeType mimeType = QMimeDatabase().mimeTypeForFile(item.trashPath);

                if (mimeType.isValid())
                {
                    pix = QIcon::fromTheme(mimeType.genericIconName()).pixmap(128);
                }
            }

            return pix;
        }

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))

        QVariant var = QPixmap();

        return var;

#else

        return QVariant(QVariant::Pixmap);

#endif

    }

    if ((role == Qt::ToolTipRole) && (index.column() == DTrashRelPath))
    {
        return (item.collectionRelativePath);
    }

    switch (index.column())
    {
        case DTrashRelPath:
        {
            return (item.collectionRelativePath);
        }

        case DTrashTimeStamp:
        {
            QString dateTimeFormat = QLocale().dateTimeFormat();

            if (!dateTimeFormat.contains(QLatin1String("yyyy")))
            {
                dateTimeFormat.replace(QLatin1String("yy"),
                                       QLatin1String("yyyy"));
            }

            return (item.deletionTimestamp.toString(dateTimeFormat));
        }

        default:
        {
            return QVariant();
        }
    };
}

void DTrashItemModel::sort(int column, Qt::SortOrder order)
{
    d->sortColumn = column;
    d->sortOrder  = order;

    if (d->data.count() < 2)
    {
        return;
    }

    std::sort(d->data.begin(), d->data.end(),
        [column, order](const DTrashItemInfo& a, const DTrashItemInfo& b)
        {
            if ((column == DTrashTimeStamp) && (a.deletionTimestamp != b.deletionTimestamp))
            {
                if (order == Qt::DescendingOrder)
                {
                    return (a.deletionTimestamp > b.deletionTimestamp);
                }
                else
                {
                    return (a.deletionTimestamp < b.deletionTimestamp);
                }
            }

            if (order == Qt::DescendingOrder)
            {
                return (a.collectionRelativePath > b.collectionRelativePath);
            }

            return (a.collectionRelativePath < b.collectionRelativePath);
        }
    );

    const QModelIndex topLeft     = index(0, 0);
    const QModelIndex bottomRight = index(rowCount(QModelIndex())    - 1,
                                          columnCount(QModelIndex()) - 1);

    Q_EMIT dataChanged(topLeft, bottomRight);
}

bool DTrashItemModel::pixmapForItem(const QString& path, QPixmap& pix) const
{
    double ratio  = qApp->devicePixelRatio();
    int thumbSize = qMin(qRound((double)d->thumbSize * ratio), (int)ThumbnailSize::HD);

    bool ret      = d->thumbnailThread->find(ThumbnailIdentifier(path), pix, thumbSize);
    pix.setDevicePixelRatio(ratio);

    return ret;
}

QVariant DTrashItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    switch (section)
    {
        case DTrashThumb:
        {
            return i18n("Thumbnail");
        }

        case DTrashRelPath:
        {
            return i18n("Relative Path");
        }

        case DTrashTimeStamp:
        {
            return i18n("Deletion Time");
        }

        default:
        {
            return QVariant();
        }
    }
}

void DTrashItemModel::append(const DTrashItemInfo& itemInfo)
{
    if (d->itemsLoadingThread != sender())
    {
        return;
    }

    beginInsertRows(QModelIndex(), d->data.count(), d->data.count());
    d->data.append(itemInfo);
    endInsertRows();

    sort(d->sortColumn, d->sortOrder);

    Q_EMIT dataChange();
}

void DTrashItemModel::removeItems(const QModelIndexList& indexes)
{
    QList<QPersistentModelIndex> persistentIndexes;

    Q_FOREACH (const QModelIndex& index, indexes)
    {
        persistentIndexes << index;
    }

    Q_EMIT layoutAboutToBeChanged();

    Q_FOREACH (const QPersistentModelIndex& index, persistentIndexes)
    {
        if (!index.isValid())
        {
            continue;
        }

        const DTrashItemInfo& item = d->data[index.row()];

        d->failedThumbnails.removeAll(item.collectionPath);
        d->failedThumbnails.removeAll(item.trashPath);

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        removeRow(index.row());
        d->data.removeAt(index.row());
        endRemoveRows();
    }

    Q_EMIT layoutChanged();
    Q_EMIT dataChange();
}

void DTrashItemModel::refreshLayout()
{
    const QModelIndex topLeft     = index(0, 0);
    const QModelIndex bottomRight = index(rowCount(QModelIndex()) - 1, 0);

    Q_EMIT dataChanged(topLeft, bottomRight);
    Q_EMIT layoutAboutToBeChanged();
    Q_EMIT layoutChanged();
}

void DTrashItemModel::refreshThumbnails(const LoadingDescription& desc, const QPixmap& pix)
{
    if (pix.isNull())
    {
        if (!d->failedThumbnails.contains(desc.filePath))
        {
            d->failedThumbnails << desc.filePath;
        }
    }

    if (d->collectionThumbs.contains(desc.filePath))
    {
        d->collectionThumbs.removeAll(desc.filePath);
        d->failedThumbnails << desc.filePath;
    }

    const QModelIndex topLeft     = index(0, 0);
    const QModelIndex bottomRight = index(rowCount(QModelIndex()) - 1, 0);

    Q_EMIT dataChanged(topLeft, bottomRight);
}

void DTrashItemModel::clearCurrentData()
{
    d->failedThumbnails.clear();
    beginResetModel();
    d->data.clear();
    endResetModel();

    Q_EMIT dataChange();
}

void DTrashItemModel::loadItemsForCollection(const QString& colPath)
{
    Q_EMIT signalLoadingStarted();

    d->trashAlbumPath = colPath;

    clearCurrentData();

    d->itemsLoadingThread = IOJobsManager::instance()->startDTrashItemsListingForCollection(colPath);

    connect(d->itemsLoadingThread, SIGNAL(collectionTrashItemInfo(DTrashItemInfo)),
            this, SLOT(append(DTrashItemInfo)),
            Qt::QueuedConnection);

    connect(d->itemsLoadingThread, SIGNAL(signalFinished()),
            this, SIGNAL(signalLoadingFinished()),
            Qt::QueuedConnection);
}

DTrashItemInfo DTrashItemModel::itemForIndex(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return DTrashItemInfo();
    }

    return d->data.at(index.row());
}

DTrashItemInfoList DTrashItemModel::itemsForIndexes(const QList<QModelIndex>& indexes)
{
    DTrashItemInfoList items;

    Q_FOREACH (const QModelIndex& index, indexes)
    {
        if (!index.isValid())
        {
            continue;
        }

        items << itemForIndex(index);
    }

    return items;
}

QModelIndex DTrashItemModel::indexForItem(const DTrashItemInfo& itemInfo) const
{
    int index = d->data.indexOf(itemInfo);

    if (index != -1)
    {
        return createIndex(index, 0);
    }

    return QModelIndex();
}

DTrashItemInfoList DTrashItemModel::allItems()
{
    return d->data;
}

bool DTrashItemModel::isEmpty()
{
    return d->data.isEmpty();
}

void DTrashItemModel::changeThumbSize(int size)
{
    d->thumbSize = size;

    if (isEmpty())
    {
        return;
    }

    QTimer::singleShot(100, this, SLOT(refreshLayout()));
}

QString DTrashItemModel::trashAlbumPath() const
{
    return d->trashAlbumPath;
}

} // namespace Digikam
