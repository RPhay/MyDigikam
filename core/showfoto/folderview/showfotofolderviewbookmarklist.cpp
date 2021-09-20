/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmark places
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

// KDE includes

#include <klocalizedstring.h>

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
    {
    }

public:
};

ShowfotoFolderViewBookmarkList::ShowfotoFolderViewBookmarkList(QWidget* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
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
}

ShowfotoFolderViewBookmarkList::~ShowfotoFolderViewBookmarkList()
{
    delete d;
}

} // namespace ShowFoto
