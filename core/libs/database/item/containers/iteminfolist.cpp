/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Container for image info objects
 *
 * SPDX-FileCopyrightText: 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2007-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "iteminfolist.h"

// Local includes


namespace Digikam
{

ItemInfoList::ItemInfoList(const QList<ItemInfo>& list)
    : QList<ItemInfo>(list)
{
}

ItemInfoList::ItemInfoList(const QList<qlonglong>& idList)
{
    for (const qlonglong& id : std::as_const(idList))
    {
        append(ItemInfo(id));
    }
}

QList<qlonglong> ItemInfoList::toImageIdList() const
{
    QList<qlonglong> idList;

    for (const ItemInfo& info : std::as_const(*this))
    {
        idList << info.id();
    }

    return idList;
}

QList<QUrl> ItemInfoList::toImageUrlList() const
{
    QList<QUrl> urlList;

    for (const ItemInfo& info : std::as_const(*this))
    {
        urlList << info.fileUrl();
    }

    return urlList;
}

bool ItemInfoList::namefileLessThan(const ItemInfo& d1, const ItemInfo& d2)
{
    return d1.name().toLower() < d2.name().toLower(); // sort by name
}

ItemInfo ItemInfoList::singleGroupMainItem() const
{
    if (length() == 1)
    {
        return first();
    }

    ItemInfo mainItem;
    ItemInfoList grouped;

    if      (first().isGrouped())
    {
        mainItem = first().groupImage();

        if (!this->contains(mainItem))
        {
            return ItemInfo();
        }
    }
    else if (first().hasGroupedImages())
    {
        mainItem = first();
    }
    else
    {
        return ItemInfo();
    }

    grouped << mainItem << mainItem.groupedImages();

    for (const ItemInfo& info : std::as_const(*this))
    {
        if (!grouped.contains(info))
        {
            return ItemInfo();
        }
    }

    return mainItem;
}

} // namespace Digikam
