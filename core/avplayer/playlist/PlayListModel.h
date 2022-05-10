/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_PLAYER_PLAYLISTMODEL_H
#define QTAV_PLAYER_PLAYLISTMODEL_H

// Qt includes

#include <QAbstractListModel>

// Local includes

#include "PlayListItem.h"

namespace AVPlayer
{

class PlayListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(PlayListModel)

public:

    explicit PlayListModel(QObject* parent = nullptr);

    QList<PlayListItem> items() const;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

    void updateLayout();

private:

    QList<PlayListItem> mItems;
};

} // namespace AVPlayer

#endif // QTAV_PLAYER_PLAYLISTMODEL_H
