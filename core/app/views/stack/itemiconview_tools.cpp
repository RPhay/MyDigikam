/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Tools methods.
 *
 * SPDX-FileCopyrightText: 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2002-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2011-2013 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemiconview_p.h"

namespace Digikam
{

void ItemIconView::slotEditor()
{
    const ItemInfoList& imageInfoList = selectedInfoList(ApplicationSettings::Tools);
    ItemInfo singleInfo               = currentInfo();

    if (singleInfo.isNull() && !imageInfoList.isEmpty())
    {
        singleInfo = imageInfoList.first();
    }

    Album* const current = currentAlbum();
    d->utilities->openInfos(singleInfo, imageInfoList, current);
}

void ItemIconView::slotFileWithDefaultApplication()
{
    d->utilities->openInfosWithDefaultApplication(selectedInfoList(ApplicationSettings::Tools));
}

void ItemIconView::slotLightTable()
{
    bool grouping                    = selectedNeedGroupResolving(ApplicationSettings::LightTable);
    const ItemInfoList& selectedList = selectedInfoList(false, grouping);

    if (selectedList.isEmpty())
    {
        grouping = allNeedGroupResolving(ApplicationSettings::LightTable);
    }

    const ItemInfoList& allInfoList = allInfo(grouping);
    const ItemInfo currentItemInfo  = currentInfo();

    d->utilities->insertToLightTableAuto(allInfoList, selectedList, currentItemInfo);
}

void ItemIconView::slotQueueMgr()
{
    bool grouping                     = selectedNeedGroupResolving(ApplicationSettings::BQM);
    const ItemInfoList& imageInfoList = selectedInfoList(false, grouping);
    ItemInfo singleInfo                = currentInfo();

    if (singleInfo.isNull() && !imageInfoList.isEmpty())
    {
        singleInfo = imageInfoList.first();
    }

    if (singleInfo.isNull())
    {
        grouping                     = allNeedGroupResolving(ApplicationSettings::BQM);
        const ItemInfoList& allItems = allInfo(grouping);

        if (!allItems.isEmpty())
        {
            singleInfo = allItems.first();
        }
    }

    d->utilities->insertToQueueManager(imageInfoList, singleInfo, true);
}

void ItemIconView::slotImageEdit()
{
    // Where is the difference to slotEditor?

    slotEditor();
}

void ItemIconView::slotImageLightTable()
{
    const ItemInfoList& selectedList = selectedInfoList(ApplicationSettings::LightTable);
    const ItemInfo currentItemInfo   = currentInfo();

    // replace images in light table

    d->utilities->insertToLightTable(selectedList, currentItemInfo, false);
}

void ItemIconView::slotImageAddToLightTable()
{
    const ItemInfoList& selectedList = selectedInfoList(ApplicationSettings::LightTable);
    const ItemInfo currentItemInfo   = currentInfo();

    // add to images in light table

    d->utilities->insertToLightTable(selectedList, currentItemInfo, true);
}

void ItemIconView::slotImageAddToCurrentQueue()
{
    const ItemInfoList& selectedList = selectedInfoList(ApplicationSettings::BQM);
    const ItemInfo currentItemInfo   = currentInfo();

    d->utilities->insertToQueueManager(selectedList, currentItemInfo, false);
}

void ItemIconView::slotImageAddToNewQueue()
{
    const bool newQueue              = QueueMgrWindow::queueManagerWindowCreated() &&
                    !QueueMgrWindow::queueManagerWindow()->queuesMap().isEmpty();

    const ItemInfoList& selectedList = selectedInfoList(ApplicationSettings::BQM);
    const ItemInfo currentItemInfo   = currentInfo();

    d->utilities->insertToQueueManager(selectedList, currentItemInfo, newQueue);
}

void ItemIconView::slotImageAddToExistingQueue(int queueid)
{
    const ItemInfoList& selectedList = selectedInfoList(ApplicationSettings::BQM);
    const ItemInfo currentItemInfo   = currentInfo();

    if (!selectedList.isEmpty())
    {
        d->utilities->insertSilentToQueueManager(selectedList, currentItemInfo, queueid);
    }
}

} // namespace Digikam
