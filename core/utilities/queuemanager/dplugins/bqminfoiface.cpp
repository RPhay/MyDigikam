/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-06
 * Description : interface to database information for BQM tools.
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "bqminfoiface.h"

// Local includes

#include "queuemgrwindow.h"
#include "queuepool.h"
#include "queuelist.h"
#include "iteminfoset.h"

namespace Digikam
{

BqmInfoIface::BqmInfoIface(QObject* const parent)
    : DBInfoIface(parent)
{
    setObjectName(QLatin1String("BqmInfoIface"));
}

QStringList BqmInfoIface::pendingItemPathsFromCurrentQueue() const
{
    return itemPathsFromCurrentQueue(QueueListView::Pending);
}

QStringList BqmInfoIface::selectedItemPathsFromCurrentQueue() const
{
    return itemPathsFromCurrentQueue(QueueListView::Selected);
}

QStringList BqmInfoIface::allItemPathsFromCurrentQueue() const
{
    return itemPathsFromCurrentQueue(QueueListView::All);
}

QStringList BqmInfoIface::itemPathsFromCurrentQueue(int type) const
{
    QStringList paths;

    QueuePool* const pool   = QueueMgrWindow::queueManagerWindow()->queuePool();
    QueuePoolItemsList list = pool->itemsList(pool->currentIndex(), type);

    Q_FOREACH (const ItemInfoSet& item, list)
    {
        paths << item.info.filePath();
    }

    return paths;
}

} // namespace Digikam

#include "moc_bqminfoiface.cpp"
