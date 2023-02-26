/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef AV_PLAYER_PLAYLIST_MODEL_H
#define AV_PLAYER_PLAYLIST_MODEL_H

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

    explicit PlayListModel(QObject* const parent = nullptr);

    QList<PlayListItem> items()                             const;

    virtual Qt::ItemFlags flags(const QModelIndex& index)   const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role)       const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

    void updateLayout();

private:

    QList<PlayListItem> mItems;
};

} // namespace AVPlayer

#endif // AV_PLAYER_PLAYLIST_MODEL_H
