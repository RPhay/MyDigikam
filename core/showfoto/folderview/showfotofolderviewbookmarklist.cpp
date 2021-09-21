/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmarks list
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

#include "showfotofolderviewbookmarklist.h"

// Qt includes

#include <QTreeWidget>
#include <QHeaderView>
#include <QMimeData>
#include <QFileInfo>
#include <QUrl>
#include <QList>
#include <QDir>
#include <QDrag>
#include <QMenu>

// KDE includes

#include <klocalizedstring.h>

// Local include

#include "digikam_debug.h"
#include "showfotofolderviewbookmarks.h"

namespace ShowFoto
{

ShowfotoFolderViewBookmarkItem::ShowfotoFolderViewBookmarkItem(QTreeWidgetItem* const parent)
    : QTreeWidgetItem(parent)
{
    setDisabled(false);
    setSelected(false);
}

ShowfotoFolderViewBookmarkItem::~ShowfotoFolderViewBookmarkItem()
{
}

void ShowfotoFolderViewBookmarkItem::setPath(const QString& path)
{
    m_path = path;
}

QString ShowfotoFolderViewBookmarkItem::path() const
{
    return m_path;
}

// ---------------------------------------------------------------------

class Q_DECL_HIDDEN ShowfotoFolderViewBookmarkList::Private
{
public:

    explicit Private()
        : ctxmenu(nullptr),
          parent (nullptr)
    {
    }

public:

    QMenu*                       ctxmenu;
    ShowfotoFolderViewBookmarks* parent;
};

ShowfotoFolderViewBookmarkList::ShowfotoFolderViewBookmarkList(ShowfotoFolderViewBookmarks* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    d->parent = parent;
    setWhatsThis(i18nc("@info", "You can add or remove bookmarked places here."));
    setAlternatingRowColors(true);
    setColumnCount(1);
    setHeaderHidden(true);
    setSortingEnabled(true);
    setAllColumnsShowFocus(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::SingleSelection);
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAcceptDrops(true);
    setDragEnabled(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);
    viewport()->setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    // --- Populate context menu

    d->ctxmenu = new QMenu(this);
    d->ctxmenu->setTitle(i18nc("@title", "Bookmarks"));
    d->ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("AddBookmark")));
    d->ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("DelBookmark")));
    d->ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("EditBookmark")));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotContextMenu(QPoint)));
}

ShowfotoFolderViewBookmarkList::~ShowfotoFolderViewBookmarkList()
{
    delete d;
}

void ShowfotoFolderViewBookmarkList::slotContextMenu(const QPoint& pos)
{
    ShowfotoFolderViewBookmarkItem* const fvitem = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(itemAt(pos));

    if (fvitem && (fvitem->parent() == d->parent->topBookmarksItem()))
    {
        d->ctxmenu->exec(mapToGlobal(pos));
    }
}

void ShowfotoFolderViewBookmarkList::dragEnterEvent(QDragEnterEvent* e)
{
    QTreeWidget::dragEnterEvent(e);
    e->accept();
}

void ShowfotoFolderViewBookmarkList::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> urls = e->mimeData()->urls();

        QFileInfo inf(urls.first().toLocalFile());

        if (inf.isDir())
        {
            QTreeWidgetItem* const item                  = itemAt(e->pos());
            ShowfotoFolderViewBookmarkItem* const fvitem = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(item);

            if (
                (item == d->parent->topBookmarksItem()) ||
                (fvitem && (fvitem->parent() == d->parent->topBookmarksItem()))
               )
            {
                QTreeWidget::dragMoveEvent(e);
                e->accept();
                return;
            }
        }
    }

    e->ignore();
}

void ShowfotoFolderViewBookmarkList::dropEvent(QDropEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> urls = e->mimeData()->urls();

        QFileInfo inf(urls.first().toLocalFile());

        if (inf.isDir())
        {
            QTreeWidgetItem* const item                  = itemAt(e->pos());
            ShowfotoFolderViewBookmarkItem* const fvitem = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(item);

            if (
                (item == d->parent->topBookmarksItem()) ||
                (fvitem && (fvitem->parent() == d->parent->topBookmarksItem()))
               )
            {
                QTreeWidget::dropEvent(e);

                QString path = inf.filePath();

                if (!path.endsWith(QDir::separator()))
                {
                    path.append(QDir::separator());
                }

                emit signalAddBookmark(path);

                e->acceptProposedAction();
                return;
            }
        }
    }

    e->ignore();
}

// cppcheck-suppress passedByValue
QMimeData* ShowfotoFolderViewBookmarkList::mimeData(const QList<QTreeWidgetItem*> items) const       // clazy:exclude=function-args-by-ref
{
    QList<QUrl> urls;

    foreach (QTreeWidgetItem* const itm, items)
    {
        ShowfotoFolderViewBookmarkItem* const vitem = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(itm);

        if (vitem)
        {
            urls.append(QUrl::fromLocalFile(vitem->path()));
        }
    }

    QMimeData* const data = new QMimeData;
    data->setUrls(urls);

    return data;
}

void ShowfotoFolderViewBookmarkList::startDrag(Qt::DropActions /*supportedActions*/)
{
    QList<QTreeWidgetItem*> items = selectedItems();

    if (items.isEmpty())
    {
        return;
    }

    QDrag* const drag = new QDrag(this);
    drag->setMimeData(mimeData(items));
    drag->exec();
}

} // namespace ShowFoto
