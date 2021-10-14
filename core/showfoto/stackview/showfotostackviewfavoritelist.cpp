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
#include "showfotostackviewfavorites.h"
#include "showfotostackviewfavoriteitem.h"
#include "dfileoperations.h"

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewFavoriteList::Private
{
public:

    explicit Private()
        : parent (nullptr)
    {
    }

public:

    ShowfotoStackViewFavorites* parent;
};

ShowfotoStackViewFavoriteList::ShowfotoStackViewFavoriteList(ShowfotoStackViewFavorites* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    d->parent = parent;
    setWhatsThis(i18nc("@info", "You can add or remove favorite stacks here."));
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
        QStringList lst = fvitem->urlsToPaths();
        emit signalLoadContentsFromFiles(lst, (!lst.isEmpty() ? lst.first() : QString()));
    }
}

void ShowfotoStackViewFavoriteList::slotContextMenu(const QPoint& pos)
{
    ShowfotoStackViewFavoriteItem* const fvitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(itemAt(pos));

    if (fvitem && (fvitem->parent() == d->parent->topFavoritesItem()))
    {
        QMenu* const ctxmenu        = new QMenu(this);
        ctxmenu->setTitle(i18nc("@title", "Favorites"));
        ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("AddFavorite")));
        ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("DelFavorite")));
        ctxmenu->addAction(d->parent->toolBarAction(QLatin1String("EditFavorite")));
        ctxmenu->addSeparator();
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

        ctxmenu->exec(mapToGlobal(pos));

        delete ctxmenu;
    }
}

void ShowfotoStackViewFavoriteList::dragEnterEvent(QDragEnterEvent* e)
{
    QTreeWidget::dragEnterEvent(e);
    e->accept();
}

void ShowfotoStackViewFavoriteList::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> mimeurls = e->mimeData()->urls();
        QList<QUrl> urls;

        foreach (const QUrl& url, mimeurls)
        {
            QFileInfo info(url.toLocalFile());

            if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
            {
                QString path    = info.absoluteFilePath();
                QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);
                QString suffix  = info.suffix().toUpper();

                if (mtype.name().startsWith(QLatin1String("image/")) ||
                    (suffix == QLatin1String("PGF"))                 ||
                    (suffix == QLatin1String("KRA"))                 ||
                    (suffix == QLatin1String("CR3"))                 ||
                    (suffix == QLatin1String("HEIC"))                ||
                    (suffix == QLatin1String("HEIF")))
                {
                    urls << url;
                }
            }
        }

        if (!urls.isEmpty())
        {
            QTreeWidgetItem* const item                 = itemAt(e->pos());
            ShowfotoStackViewFavoriteItem* const fvitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(item);

            if (
                (item == d->parent->topFavoritesItem()) ||
                (fvitem && (fvitem->parent() == d->parent->topFavoritesItem()))
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

void ShowfotoStackViewFavoriteList::dropEvent(QDropEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> mimeurls = e->mimeData()->urls();
        QList<QUrl> urls;

        foreach (const QUrl& url, mimeurls)
        {
            QFileInfo info(url.toLocalFile());

            if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
            {
                QString path    = info.absoluteFilePath();
                QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);
                QString suffix  = info.suffix().toUpper();

                if (mtype.name().startsWith(QLatin1String("image/")) ||
                    (suffix == QLatin1String("PGF"))                 ||
                    (suffix == QLatin1String("KRA"))                 ||
                    (suffix == QLatin1String("CR3"))                 ||
                    (suffix == QLatin1String("HEIC"))                ||
                    (suffix == QLatin1String("HEIF")))
                {
                    urls << url;
                }
            }
        }

        if (!urls.isEmpty())
        {
            QTreeWidgetItem* const item                 = itemAt(e->pos());
            ShowfotoStackViewFavoriteItem* const fvitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(item);

            if (
                (item == d->parent->topFavoritesItem()) ||
                (fvitem && (fvitem->parent() == d->parent->topFavoritesItem()))
               )
            {
                QTreeWidget::dropEvent(e);

                emit signalAddFavorite(urls);

                e->acceptProposedAction();
                return;
            }
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

void ShowfotoStackViewFavoriteList::startDrag(Qt::DropActions /*supportedActions*/)
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

ShowfotoStackViewFavoriteItem* ShowfotoStackViewFavoriteList::favoriteExists(const QString& name) const
{
    bool found                          = false;
    ShowfotoStackViewFavoriteItem* item = nullptr;

    for (int i = 0 ; i < d->parent->topFavoritesItem()->childCount() ; ++i)
    {
        item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(d->parent->topFavoritesItem()->child(i));

        if (name == item->name())
        {
            found = true;
            break;
        }
    }

    if (found)
    {
        return item;
    }

    return nullptr;
}

} // namespace ShowFoto
