/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorites list
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SHOWFOTO_STACK_VIEW_FAVORITE_LIST_H
#define SHOWFOTO_STACK_VIEW_FAVORITE_LIST_H

// Qt includes

#include <QList>
#include <QUrl>
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

class ShowfotoStackViewFavorites;
class ShowfotoStackViewFavoriteItem;

class ShowfotoStackViewFavoriteList : public QTreeWidget
{
    Q_OBJECT

public:

    explicit ShowfotoStackViewFavoriteList(ShowfotoStackViewFavorites* const parent);
    ~ShowfotoStackViewFavoriteList()                                         override;

    ShowfotoStackViewFavoriteItem* favoriteExists(const QString& name) const;

Q_SIGNALS:

    void signalAddFavorite();
    void signalAddFavorite(const QList<QUrl>&);
    void signalLoadContentsFromFiles(const QStringList& files);

public Q_SLOTS:

    void slotLoadContents();

private Q_SLOTS:

    void slotContextMenu(const QPoint& pos);
    void slotOpenInFileManager();

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

#endif // SHOWFOTO_STACK_VIEW_FAVORITE_LIST_H
