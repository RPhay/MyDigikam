/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-06-09
 * Description : A model to list the tracks
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2014      by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_TRACK_LISTMODEL_H
#define DIGIKAM_TRACK_LISTMODEL_H

// Qt includes

#include <QAbstractItemModel>

// Local includes

#include "trackmanager.h"

namespace Digikam
{

class TrackListModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    explicit TrackListModel(TrackManager* const trackManager, QObject* const parent);
    ~TrackListModel() override;

    // QAbstractItemModel:

    int columnCount(const QModelIndex& parent = QModelIndex() )                                     const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role)                               override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)                             const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex())               const override;
    QModelIndex parent(const QModelIndex& index)                                                    const override;
    int rowCount(const QModelIndex& parent = QModelIndex())                                         const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)         override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)                         const override;
    Qt::ItemFlags flags(const QModelIndex& index)                                                   const override;

    TrackManager::Track getTrackForIndex(const QModelIndex& index)                                  const;

private Q_SLOTS:

    void slotTrackManagerUpdated();

private:

    class Private;
    const QScopedPointer<Private> d;
};

} // namespace Digikam

#endif // DIGIKAM_TRACK_LISTMODEL_H
