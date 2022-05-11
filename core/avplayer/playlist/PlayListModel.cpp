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

#include "PlayListModel.h"

// Qt includes

#include <QVector>

// Local includes

#include "PlayListItem.h"

namespace AVPlayer
{

PlayListModel::PlayListModel(QObject* const parent)
    : QAbstractListModel(parent)
{
}

QList<PlayListItem> PlayListModel::items() const
{
    return mItems;
}

Qt::ItemFlags PlayListModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return (QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled);

    return (QAbstractListModel::flags(index) | Qt::ItemIsSelectable);
}

int PlayListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return mItems.size();
}

QVariant PlayListModel::data(const QModelIndex& index, int role) const
{
    QVariant v;

    if ((index.row() < 0) || (index.row() >= mItems.size()))
        return v;

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
        v.setValue(mItems.at(index.row()));

    return v;
}

bool PlayListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if ((index.row() >= 0) && (index.row() < mItems.size()) &&
        ((role == Qt::EditRole) || (role == Qt::DisplayRole)))
    {
        // TODO: compare value?

        mItems.replace(index.row(), value.value<PlayListItem>());

        emit dataChanged(index, index, QVector<int>() << role);

        return true;
    }

    return false;
}

bool PlayListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if ((count < 1) || (row < 0) || (row > rowCount(parent)))
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int r = 0 ; r < count ; ++r)
        mItems.insert(row, PlayListItem());

    endInsertRows();

    return true;
}

bool PlayListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if ((count <= 0) || (row < 0) || ((row + count) > rowCount(parent)))
        return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int r = 0 ; r < count ; ++r)
        mItems.removeAt(row);

    endRemoveRows();

    return true;
}

void PlayListModel::updateLayout()
{
    emit layoutChanged();
}

} // namespace AVPlayer
