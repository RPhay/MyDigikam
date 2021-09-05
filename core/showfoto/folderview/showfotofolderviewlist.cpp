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
#include <QScrollBar>
#include <QContextMenuEvent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewList::Private
{

public:

    explicit Private()
      : fsmenu      (nullptr),
        view        (nullptr)
    {
    }

    QMenu*                     fsmenu;
    ShowfotoFolderViewSideBar* view;
};

ShowfotoFolderViewList::ShowfotoFolderViewList(ShowfotoFolderViewSideBar* const view)
    : QListView(view),
      d        (new Private)
{
    d->view   = view;

    setObjectName(QLatin1String("ShowfotoFolderViewList"));
    setAlternatingRowColors(true);

    // --- Populate context menu

    d->fsmenu = new QMenu(this);
    d->fsmenu->setTitle(i18nc("@title", "Folder-View Options"));
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("go-previous")),
                         i18nc("menu", "Go to Previous"),
                         d->view,
                         SLOT(slotUndo()));
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("go-next")),
                         i18nc("menu", "Go to Next"),
                         d->view,
                         SLOT(slotRedo()));
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("go-home")),
                         i18nc("menu", "Go Home"),
                         d->view,
                         SLOT(slotGoHome()));
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("go-up")),
                         i18nc("menu", "Go Up"),
                         d->view,
                         SLOT(slotGoUp()));

    d->fsmenu->addSeparator(),

    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("folder-download")),
                         i18nc("menu", "Load Contents"),
                         d->view,
                         SLOT(slotLoadContents()));
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
