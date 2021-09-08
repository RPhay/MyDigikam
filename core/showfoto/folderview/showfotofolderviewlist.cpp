/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : List-view for the Showfoto folder view.
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

#include "showfotofolderviewlist.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QFileInfo>
#include <QHeaderView>
#include <QDir>
#include <QMenu>
#include <QMimeData>
#include <QScrollBar>
#include <QContextMenuEvent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotofolderviewbar.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewList::Private
{

public:

    explicit Private()
      : fsmenu      (nullptr),
        view        (nullptr),
        bar         (nullptr)
    {
    }

    QMenu*                     fsmenu;
    ShowfotoFolderViewSideBar* view;
    ShowfotoFolderViewBar*     bar;
};

ShowfotoFolderViewList::ShowfotoFolderViewList(ShowfotoFolderViewSideBar* const view,
                                               ShowfotoFolderViewBar* const bar)
    : QListView(view),
      d        (new Private)
{
    d->view   = view;
    d->bar    = bar;

    setObjectName(QLatin1String("ShowfotoFolderViewList"));
    setAlternatingRowColors(true);
    setViewMode(QListView::ListMode);
    setDragEnabled(true);
    setIconSize(QSize(32, 32));
    setDragDropMode(QAbstractItemView::DragOnly);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // --- Populate context menu

    d->fsmenu = new QMenu(this);
    d->fsmenu->setTitle(i18nc("@title", "Folder-View Options"));
    d->fsmenu->addAction(d->bar->toolBarAction(QLatin1String("GoPrevious")));
    d->fsmenu->addAction(d->bar->toolBarAction(QLatin1String("GoNext")));
    d->fsmenu->addAction(d->bar->toolBarAction(QLatin1String("GoHome")));
    d->fsmenu->addAction(d->bar->toolBarAction(QLatin1String("GoUp")));
    d->fsmenu->addSeparator(),
    d->fsmenu->addAction(d->bar->toolBarAction(QLatin1String("LoadContents")));
}

ShowfotoFolderViewList::~ShowfotoFolderViewList()
{
    delete d;
}

void ShowfotoFolderViewList::contextMenuEvent(QContextMenuEvent* e)
{
    d->fsmenu->exec(e->globalPos());

    QListView::contextMenuEvent(e);
}

void ShowfotoFolderViewList::mouseDoubleClickEvent(QMouseEvent* e)
{
    d->view->loadContents(currentIndex());

    QListView::mouseDoubleClickEvent(e);
}

} // namespace ShowFoto
