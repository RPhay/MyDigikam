/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-02-14
 * Description : Table view shared object
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
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

#include "tableview_shared.h"

namespace Digikam
{

TableViewShared::TableViewShared()
    : imageModel                    (nullptr),
      imageFilterModel              (nullptr),
      imageFilterSelectionModel     (nullptr),
      thumbnailLoadThread           (nullptr),
      tableView                     (nullptr),
      treeView                      (nullptr),
      tableViewModel                (nullptr),
      tableViewSelectionModel       (nullptr),
      tableViewSelectionModelSyncer (nullptr),
      columnFactory                 (nullptr),
      itemDelegate                  (nullptr),
      isActive                      (false)
{
}

TableViewShared::~TableViewShared()
{
}

} // namespace Digikam
