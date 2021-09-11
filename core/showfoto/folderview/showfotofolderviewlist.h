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

#include <QTreeView>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QModelIndex>

// Local includes

#include "showfotofolderviewsidebar.h"

namespace ShowFoto
{

class ShowfotoFolderViewBar;

class ShowfotoFolderViewList : public QTreeView
{
    Q_OBJECT

public:

    enum ViewMode
    {
        ShortView = 0,
        DetailledView
    };

public:

    explicit ShowfotoFolderViewList(ShowfotoFolderViewSideBar* const view,
                                    ShowfotoFolderViewBar* const bar);
    ~ShowfotoFolderViewList()                                           override;

    void setEnableToolTips(bool val);

public Q_SLOTS:

    void slotIconSizeChanged(int);

private:

    void mouseDoubleClickEvent(QMouseEvent*)                            override;
    void contextMenuEvent(QContextMenuEvent*)                           override;
    void mouseMoveEvent(QMouseEvent*)                                   override;
    void wheelEvent(QWheelEvent*)                                       override;
    void keyPressEvent(QKeyEvent*)                                      override;
    void focusOutEvent(QFocusEvent*)                                    override;
    void leaveEvent(QEvent*)                                            override;


    void hideToolTip();
    bool acceptToolTip(const QModelIndex& pos)                  const;

private Q_SLOTS:

    void slotToolTip();
    void slotOpenInFileManager();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_LIST_H
