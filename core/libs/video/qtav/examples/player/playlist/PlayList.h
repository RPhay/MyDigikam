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


#ifndef QTAV_PLAYER_PLAYLIST_H
#define QTAV_PLAYER_PLAYLIST_H

#include <QWidget>
#include <QModelIndex>

#include "PlayListItem.h"

QT_BEGIN_NAMESPACE
class QListView;
class QToolButton;
QT_END_NAMESPACE
class PlayListDelegate;

class PlayListModel;
class PlayList : public QWidget
{
    Q_OBJECT
public:
    explicit PlayList(QWidget *parent = 0);
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
    int maxRows() const;

signals:
    void aboutToPlay(const QString& url);

private slots:
    void removeSelectedItems();
    void clearItems();
    //
    void addItems();

    void onAboutToPlay(const QModelIndex& index);
    //void highlight(const QModelIndex& index);
private:
    QListView *mpListView;
    QToolButton *mpClear, *mpRemove, *mpAdd;
    PlayListDelegate *mpDelegate;
    PlayListModel *mpModel;
    int mMaxRows;
    QString mFile;
    bool mFirstShow;
};

#endif // QTAV_PLAYER_PLAYLIST_H
