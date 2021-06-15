/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-21
 * Description : A model to hold GPS information about items.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_GPS_ITEM_MODEL_H
#define DIGIKAM_GPS_ITEM_MODEL_H

// Qt includes

#include <QAbstractItemModel>
#include <QPixmap>

// Local includes

#include "gpsitemcontainer.h"
#include "thumbnailloadthread.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT GPSItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    explicit GPSItemModel(QObject* const parent = nullptr);
    ~GPSItemModel()                                                                                       override;

    // own functions:

    void addItem(GPSItemContainer* const newItem);
    void setColumnCount(const int nColumns);
    GPSItemContainer* itemFromIndex(const QModelIndex& index)                                       const;
    GPSItemContainer* itemFromUrl(const QUrl& url)                                                  const;
    QModelIndex indexFromUrl(const QUrl& url)                                                       const;

    QPixmap getPixmapForIndex(const QPersistentModelIndex& itemIndex, const int size);

    // QAbstractItemModel customizations:

    int columnCount(const QModelIndex& parent = QModelIndex())                                      const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role)                               override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)                             const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex())               const override;
    QModelIndex parent(const QModelIndex& index)                                                    const override;
    int rowCount(const QModelIndex& parent = QModelIndex())                                         const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)         override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)                         const override;
    Qt::ItemFlags flags(const QModelIndex& index)                                                   const override;
    Qt::DropActions supportedDragActions()                                                          const override;

protected:

    void itemChanged(GPSItemContainer* const changedItem);

Q_SIGNALS:

    void signalThumbnailForIndexAvailable(const QPersistentModelIndex& index, const QPixmap& pixmap);

protected Q_SLOTS:

    void slotThumbnailLoaded(const LoadingDescription&, const QPixmap&);

private:

    class Private;
    Private* const d;

    friend class GPSItemContainer;
};

} // namespace Digikam

#endif // DIGIKAM_GPS_ITEM_MODEL_H
