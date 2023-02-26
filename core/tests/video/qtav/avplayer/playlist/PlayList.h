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

#ifndef AV_PLAYER_PLAYLIST_H
#define AV_PLAYER_PLAYLIST_H

// Qt includes

#include <QWidget>
#include <QModelIndex>

// Local includes

#include "PlayListItem.h"

class QListView;
class QToolButton;

namespace AVPlayer
{

class PlayListDelegate;
class PlayListModel;

class PlayList : public QWidget
{
    Q_OBJECT

public:

    explicit PlayList(QWidget* const parent = nullptr);
    ~PlayList();

    void setSaveFile(const QString& file);
    QString saveFile() const;
    void load();
    void save();

    PlayListItem itemAt(int row);
    void insertItemAt(const PlayListItem& item, int row = 0);
    void setItemAt(const PlayListItem& item, int row = 0);
    void remove(const QString& url);
    void insert(const QString& url, int row = 0);
    void setMaxRows(int r);
    int maxRows()      const;

Q_SIGNALS:

    void aboutToPlay(const QString& url);

private Q_SLOTS:

    void removeSelectedItems();
    void clearItems();

    void addItems();

    void onAboutToPlay(const QModelIndex& index);
/*
    void highlight(const QModelIndex& index);
*/
private:

    QListView*          mpListView;
    QToolButton*        mpClear;
    QToolButton*        mpRemove;
    QToolButton*        mpAdd;
    PlayListDelegate*   mpDelegate;
    PlayListModel*      mpModel;
    int                 mMaxRows;
    QString             mFile;
    bool                mFirstShow;
};

} // namespace AVPlayer

#endif // AV_PLAYER_PLAYLIST_H
