/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmarks list
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))

    QMimeData* mimeData(const QList<QTreeWidgetItem*>& items)           const override;

#else

    // cppcheck-suppress passedByValue
    QMimeData* mimeData(const QList<QTreeWidgetItem*> items)            const override;     // clazy:exclude=function-args-by-ref

#endif

    void       startDrag(Qt::DropActions supportedActions)                    override;

private:

    ShowfotoFolderViewBookmarkList(QWidget*);

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_BOOKMARK_LIST_H
