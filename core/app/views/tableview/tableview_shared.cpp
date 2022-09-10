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
