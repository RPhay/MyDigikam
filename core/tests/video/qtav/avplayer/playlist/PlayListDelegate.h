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

#ifndef AV_PLAYER_PLAYLIST_DELEGATE_H
#define AV_PLAYER_PLAYLIST_DELEGATE_H

// Qt includes

#include <QStyledItemDelegate>

namespace AVPlayer
{

class PlayListDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    explicit PlayListDelegate(QObject* const parent = nullptr);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index)                const;

private:

    mutable int        mHighlightRow;
    mutable QList<int> mSelectedRows;
};

} // namespace AVPlayer

#endif // AV_PLAYER_PLAYLIST_DELEGATE_H
