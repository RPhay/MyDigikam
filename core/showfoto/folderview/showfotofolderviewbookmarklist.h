/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmarks list
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SHOWFOTO_FOLDER_VIEW_BOOKMARK_LIST_H
#define SHOWFOTO_FOLDER_VIEW_BOOKMARK_LIST_H

// Qt includes

#include <QString>
#include <QPoint>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QContextMenuEvent>

namespace ShowFoto
{

class ShowfotoFolderViewBookmarks;
class ShowfotoFolderViewBookmarkItem;

class ShowfotoFolderViewBookmarkList : public QTreeWidget
{
    Q_OBJECT

public:

    explicit ShowfotoFolderViewBookmarkList(ShowfotoFolderViewBookmarks* const parent);
    ~ShowfotoFolderViewBookmarkList()                                         override;

    ShowfotoFolderViewBookmarkItem* bookmarkExists(const QString& path) const;
    QString bookmarkBaseName(const QString& path)                       const;

Q_SIGNALS:

    void signalAddBookmark(const QString& path);
    void signalLoadContents(const QString& path);

private Q_SLOTS:

    void slotContextMenu(const QPoint& pos);
    void slotOpenInFileManager();
    void slotLoadContents();

private:

    void dragEnterEvent(QDragEnterEvent*);
    void dragMoveEvent(QDragMoveEvent*);
    void dropEvent(QDropEvent*);

    // cppcheck-suppress passedByValue
    QMimeData* mimeData(const QList<QTreeWidgetItem*> items)            const override;
    void       startDrag(Qt::DropActions supportedActions)                    override;

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_BOOKMARK_LIST_H
