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

#ifndef SHOWFOTO_FOLDER_VIEW_LIST_H
#define SHOWFOTO_FOLDER_VIEW_LIST_H

// QT includes

#include <QListView>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QModelIndex>

// Local includes

#include "showfotofolderviewsidebar.h"

namespace ShowFoto
{

class ShowfotoFolderViewBar;

class ShowfotoFolderViewList : public QListView
{
    Q_OBJECT

public:

    explicit ShowfotoFolderViewList(ShowfotoFolderViewSideBar* const view,
                                    ShowfotoFolderViewBar* const bar);
    ~ShowfotoFolderViewList()           override;

private:

    void mouseDoubleClickEvent(QMouseEvent* e);
    void contextMenuEvent(QContextMenuEvent *e);

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_LIST_H
