/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-02-14
 * Description : Table view shared object
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TABLE_VIEW_SHARED_H
#define DIGIKAM_TABLE_VIEW_SHARED_H

class QItemSelectionModel;

namespace Digikam
{

class ItemFilterModel;
class ItemModel;
class TableView;
class TableViewColumnFactory;
class TableViewCurrentToSortedSyncer;
class TableViewItemDelegate;
class TableViewModel;
class TableViewSelectionModelSyncer;
class TableViewTreeView;
class ThumbnailLoadThread;

class TableViewShared
{
public:

    explicit TableViewShared();
    ~TableViewShared();

public:

    ItemModel*                     imageModel;
    ItemFilterModel*               imageFilterModel;
    QItemSelectionModel*           imageFilterSelectionModel;
    ThumbnailLoadThread*           thumbnailLoadThread;

    TableView*                     tableView;
    TableViewTreeView*             treeView;
    TableViewModel*                tableViewModel;
    QItemSelectionModel*           tableViewSelectionModel;

    TableViewSelectionModelSyncer* tableViewSelectionModelSyncer;
    TableViewColumnFactory*        columnFactory;
    TableViewItemDelegate*         itemDelegate;

    bool                           isActive;
};

} // namespace Digikam

#endif // DIGIKAM_TABLE_VIEW_SHARED_H
