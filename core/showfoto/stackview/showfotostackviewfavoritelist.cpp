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

#include "showfotostackviewfavoritelist.h"

// Qt includes

#include <QHeaderView>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QDrag>
#include <QMenu>
#include <QMimeType>
#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local include

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotostackviewfavorites.h"
#include "showfotostackviewfavoriteitem.h"
#include "dfileoperations.h"
#include "drawdecoder.h"

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewFavoriteList::Private
{
public:

    explicit Private()
        : parent     (nullptr),
          draggedItem(nullptr)
    {
    }

public:

    ShowfotoStackViewFavorites* parent;
    QTreeWidgetItem*            draggedItem;
    QString                     filter;
};

ShowfotoStackViewFavoriteList::ShowfotoStackViewFavoriteList(ShowfotoStackViewFavorites* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    d->parent = parent;

    setWhatsThis(i18nc("@info", "You can add or remove favorite stacks here."));
    setAlternatingRowColors(true);
    setColumnCount(1);
    setSortingEnabled(true);
    setAllColumnsShowFocus(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHeaderHidden(true);
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setContextMenuPolicy(Qt::CustomContextMenu);

    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);
    viewport()->setAcceptDrops(true);
    viewport()->setMouseTracking(true);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotContextMenu(QPoint)));
}

ShowfotoStackViewFavoriteList::~ShowfotoStackViewFavoriteList()
{
    delete d;
}

void ShowfotoStackViewFavoriteList::slotOpenInFileManager()
{
    ShowfotoStackViewFavoriteItem* const fvitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(currentItem());

    if (fvitem)
    {
        DFileOperations::openInFileManager(fvitem->urls());
    }
}

void ShowfotoStackViewFavoriteList::slotLoadContents()
{
    ShowfotoStackViewFavoriteItem* const fvitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(currentItem());

    if (fvitem)
    {
        emit signalLoadContentsFromFiles(fvitem->urlsToPaths(), fvitem->currentUrl().toLocalFile());
    }
}

void ShowfotoStackViewFavoriteList::slotContextMenu(const QPoint& pos)
{
    QMenu* const ctxmenu        = new QMenu(this);
    ctxmenu->setTitle(i18nc("@title", "Favorites"));
    ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("AddFavorite")));
    ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("AddFolder")));

    ShowfotoStackViewFavoriteItem* const fvitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(itemAt(pos));

    if (fvitem)
    {
        if (fvitem->favoriteType() != ShowfotoStackViewFavoriteItem::FavoriteRoot)
        {
            ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("DelItem")));
            ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("EditItem")));
        }

        ctxmenu->addSeparator();

        if (fvitem->favoriteType() == ShowfotoStackViewFavoriteItem::FavoriteItem)
        {
            ctxmenu->addActions(d->parent->pluginActions());
            ctxmenu->addSeparator();

            QAction* const loadContents = new QAction(QIcon::fromTheme(QLatin1String("media-playlist-normal")),
                                                      i18nc("@action: context menu", "Load Contents"), ctxmenu);
            ctxmenu->addAction(loadContents);

            connect(loadContents, SIGNAL(triggered()),
                    this, SLOT(slotLoadContents()));

            QAction* const openFileMngr = new QAction(QIcon::fromTheme(QLatin1String("folder-open")),
                                                      i18nc("@action: context menu", "Open in File Manager"), ctxmenu);
            ctxmenu->addAction(openFileMngr);

            connect(openFileMngr, SIGNAL(triggered()),
                    this, SLOT(slotOpenInFileManager()));
        }
    }

    ctxmenu->exec(mapToGlobal(pos));

    delete ctxmenu;
}

QStringList ShowfotoStackViewFavoriteList::mimeTypes() const
{
    return QStringList(QLatin1String("text/uri-list"));
}

Qt::DropActions ShowfotoStackViewFavoriteList::supportedDropActions() const
{
    return (Qt::CopyAction | Qt::MoveAction);
}

void ShowfotoStackViewFavoriteList::startDrag(Qt::DropActions /*supportedActions*/)
{
    QList<QTreeWidgetItem*> items = selectedItems();

    if (items.isEmpty())
    {
        return;
    }

    d->draggedItem    = items.first();

    QDrag* const drag = new QDrag(this);
    drag->setMimeData(mimeData(items));
    drag->exec();
}

void ShowfotoStackViewFavoriteList::dragEnterEvent(QDragEnterEvent* e)
{
    QTreeWidget::dragEnterEvent(e);
    e->accept();
}

void ShowfotoStackViewFavoriteList::dragMoveEvent(QDragMoveEvent* e)
{
    if ((e->source() == this) &&
        (dynamic_cast<ShowfotoStackViewFavoriteItem*>(itemAt(e->pos())) != d->parent->topFavoritesItem()))
    {
        QTreeWidget::dragMoveEvent(e);
        e->accept();

        return;
    }

    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> mimeurls = e->mimeData()->urls();
        QList<QUrl> urls;

        foreach (const QUrl& url, mimeurls)
        {
            if (isReadableImageFile(url.toLocalFile()))
            {
                urls << url;
            }
        }

        if (!urls.isEmpty())
        {
            QTreeWidget::dragMoveEvent(e);
            e->accept();
            return;
        }
    }

    e->ignore();
}

void ShowfotoStackViewFavoriteList::dropEvent(QDropEvent* e)
{
    if (e->source() == this)
    {
        ShowfotoStackViewFavoriteItem* const src = dynamic_cast<ShowfotoStackViewFavoriteItem*>(d->draggedItem);

        if (!src)
        {
            return;
        }

        ShowfotoStackViewFavoriteItem* const tgt = dynamic_cast<ShowfotoStackViewFavoriteItem*>(itemAt(e->pos()));

        if (!tgt)
        {
            return;
        }

        for (int i = 0 ; i < tgt->childCount() ; ++i)
        {
            ShowfotoStackViewFavoriteItem* const chl = dynamic_cast<ShowfotoStackViewFavoriteItem*>(tgt->child(i));

            if (chl && (chl->name() == src->name()))
            {
                // Ignore event if a target child item with same name than source already exist.
                return;
            }
        }

        QTreeWidget::dropEvent(e);
        e->acceptProposedAction();

        rebaseHierarchy(tgt);
        d->draggedItem = nullptr;

        return;
    }

    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> mimeurls = e->mimeData()->urls();
        QList<QUrl> urls;

        foreach (const QUrl& url, mimeurls)
        {
            if (isReadableImageFile(url.toLocalFile()))
            {
                urls << url;
            }
        }

        if (!urls.isEmpty())
        {
            QTreeWidget::dropEvent(e);

            emit signalAddFavorite(urls, urls.first());

            e->acceptProposedAction();
            return;
        }
    }

    e->ignore();
}

// cppcheck-suppress passedByValue
QMimeData* ShowfotoStackViewFavoriteList::mimeData(const QList<QTreeWidgetItem*> items) const       // clazy:exclude=function-args-by-ref
{
    QList<QUrl> urls;

    foreach (QTreeWidgetItem* const itm, items)
    {
        ShowfotoStackViewFavoriteItem* const vitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(itm);

        if (vitem)
        {
            urls.append(vitem->urls());
        }
    }

    QMimeData* const data = new QMimeData;
    data->setUrls(urls);

    return data;
}


ShowfotoStackViewFavoriteItem* ShowfotoStackViewFavoriteList::findFavoriteByHierarchy(const QString& hierarchy)
{
    ShowfotoStackViewFavoriteItem* item = nullptr;
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(*it);

        if (item && (hierarchy == item->hierarchy()))
        {
            return item;
        }

        ++it;
    }

    return nullptr;
}

void ShowfotoStackViewFavoriteList::rebaseHierarchy(ShowfotoStackViewFavoriteItem* const parent)
{
    if (parent)
    {
        QTreeWidgetItemIterator it(parent);

        while (*it)
        {
            ShowfotoStackViewFavoriteItem* const item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(*it);

            if (item)
            {
                item->setName(item->name());
            }

            ++it;
        }
    }
}

void ShowfotoStackViewFavoriteList::setFilter(const QString& filter, Qt::CaseSensitivity cs)
{
    d->filter                            = filter;
    int query                            = 0;
    ShowfotoStackViewFavoriteItem* fitem = nullptr;
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        fitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(*it);

        if (fitem && fitem->name().contains(filter, cs))
        {
            query++;

            QTreeWidgetItem* item = *it;

            do
            {
                item->setHidden(false);
                item = item->parent();
            }
            while (item);
        }
        else
        {
            fitem->setHidden(true);
        }

        ++it;
    }

    emit signalSearchResult(query);
}

QString ShowfotoStackViewFavoriteList::filter() const
{
    return d->filter;
}

} // namespace ShowFoto
