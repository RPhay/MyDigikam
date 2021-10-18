/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-21
 * Description : widget to display a list of items
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2010 by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2009-2010 by Luka Renko <lure at kubuntu dot org>
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

#include "ditemslist_p.h"

namespace Digikam
{

DItemsListView::DItemsListView(DItemsList* const parent)
    : QTreeWidget (parent),
      m_itemDraged(nullptr)
{
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setExpandsOnDoubleClick(false);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    enableDragAndDrop(true);

    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setColumnCount(8);
    setHeaderLabels(QStringList() << i18nc("@title", "Thumbnail")
                                  << i18nc("@title", "File Name")
                                  << i18nc("@title", "User1")
                                  << i18nc("@title", "User2")
                                  << i18nc("@title", "User3")
                                  << i18nc("@title", "User4")
                                  << i18nc("@title", "User5")
                                  << i18nc("@title", "User6"));
    hideColumn(User1);
    hideColumn(User2);
    hideColumn(User3);
    hideColumn(User4);
    hideColumn(User5);
    hideColumn(User6);

    header()->setSectionResizeMode(User1, QHeaderView::Interactive);
    header()->setSectionResizeMode(User2, QHeaderView::Interactive);
    header()->setSectionResizeMode(User3, QHeaderView::Interactive);
    header()->setSectionResizeMode(User4, QHeaderView::Interactive);
    header()->setSectionResizeMode(User5, QHeaderView::Interactive);
    header()->setSectionResizeMode(User6, QHeaderView::Stretch);

    connect(this, &DItemsListView::itemClicked,
            this, &DItemsListView::slotItemClicked);
}

DItemsListView::~DItemsListView()
{
}

DInfoInterface* DItemsListView::iface() const
{
    DItemsList* const p = dynamic_cast<DItemsList*>(parent());

    if (p)
    {
        return p->iface();
    }

    return nullptr;
}

void DItemsListView::enableDragAndDrop(const bool enable)
{
    setDragEnabled(enable);
    viewport()->setAcceptDrops(enable);
    setDragDropMode(enable ? QAbstractItemView::InternalMove : QAbstractItemView::NoDragDrop);
    setDragDropOverwriteMode(enable);
    setDropIndicatorShown(enable);
}

void DItemsListView::drawRow(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const
{
    DItemsListViewItem* const item = dynamic_cast<DItemsListViewItem*>(itemFromIndex(index));

    if (item && !item->hasValidThumbnail())
    {
        DItemsList* const view = dynamic_cast<DItemsList*>(parent());

        if (view)
        {
            view->updateThumbnail(item->url());
        }
    }

    QTreeWidget::drawRow(p, opt, index);
}

void DItemsListView::slotItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)

    if (!item)
    {
        return;
    }

    emit signalItemClicked(item);
}

void DItemsListView::setColumnLabel(ColumnType column, const QString& label)
{
    headerItem()->setText(column, label);
}

void DItemsListView::setColumnEnabled(ColumnType column, bool enable)
{
    if (enable)
    {
        showColumn(column);
    }
    else
    {
        hideColumn(column);
    }
}

void DItemsListView::setColumn(ColumnType column, const QString& label, bool enable)
{
    setColumnLabel(column, label);
    setColumnEnabled(column, enable);
}

DItemsListViewItem* DItemsListView::findItem(const QUrl& url)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        DItemsListViewItem* const lvItem = dynamic_cast<DItemsListViewItem*>(*it);

        if (lvItem && (lvItem->url() == url))
        {
            return lvItem;
        }

        ++it;
    }

    return nullptr;
}

QModelIndex DItemsListView::indexFromItem(DItemsListViewItem* item, int column) const
{
    return QTreeWidget::indexFromItem(item, column);
}

void DItemsListView::contextMenuEvent(QContextMenuEvent* e)
{
    QTreeWidget::contextMenuEvent(e);

    emit signalContextMenuRequested();
}

void DItemsListView::dragEnterEvent(QDragEnterEvent* e)
{
    m_itemDraged = QTreeWidget::currentItem();

    QTreeWidget::dragEnterEvent(e);

    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void DItemsListView::dragMoveEvent(QDragMoveEvent* e)
{
    QTreeWidget::dragMoveEvent(e);

    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void DItemsListView::dropEvent(QDropEvent* e)
{
    QTreeWidget::dropEvent(e);
    QList<QUrl> list = e->mimeData()->urls();
    QList<QUrl> urls;

    foreach (const QUrl& url, list)
    {
        QFileInfo fi(url.toLocalFile());

        if (fi.isFile() && fi.exists())
        {
            urls.append(url);
        }
    }

    if (!urls.isEmpty())
    {
        emit signalAddedDropedItems(urls);
    }

    scrollToItem(m_itemDraged);
}

} // namespace Digikam
