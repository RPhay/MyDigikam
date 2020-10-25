/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-10-11
 * Description : fast item sorter cache based on QCollatorSortKey
 *
 * Copyright (C) 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "itemsortercache.h"

// Qt includes

#include <QCollator>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemSorterCache::Private
{
public:

    explicit Private()
    {
    }

    QHash<QString, std::vector<QCollatorSortKey> > itemSortKeys;
    QHash<QString, std::vector<QCollatorSortKey> > albumSortKeys;

    QCollator                                      itemCollator;
    QCollator                                      albumCollator;
};

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN ItemSorterCacheCreator
{
public:

    ItemSorterCache object;
};

Q_GLOBAL_STATIC(ItemSorterCacheCreator, itemSorterCacheCreator)

// -----------------------------------------------------------------------------------------------

ItemSorterCache::ItemSorterCache()
    : d(new Private)
{
    d->itemCollator.setNumericMode(true);
    d->itemCollator.setIgnorePunctuation(false);
    d->itemCollator.setCaseSensitivity(Qt::CaseSensitive);

    d->albumCollator.setNumericMode(true);
    d->albumCollator.setIgnorePunctuation(false);
    d->albumCollator.setCaseSensitivity(Qt::CaseSensitive);
}

ItemSorterCache::~ItemSorterCache()
{
    delete d;
}

ItemSorterCache* ItemSorterCache::instance()
{
    return &itemSorterCacheCreator->object;
}

int ItemSorterCache::itemCompare(const QString& a, const QString& b,
                                 Qt::CaseSensitivity caseSensitive, bool natural) const
{
    if ((d->itemCollator.numericMode()     != natural)     ||
        (d->itemCollator.caseSensitivity() != caseSensitive))
    {
        d->itemSortKeys.clear();

        d->itemCollator.setNumericMode(natural);
        d->itemCollator.setCaseSensitivity(caseSensitive);
    }

    if (!d->itemSortKeys.contains(a))
    {
        QString as = a;
        as.replace(QLatin1String("_v"),
                   QLatin1String("vv"), Qt::CaseInsensitive);

        d->itemSortKeys[a].emplace_back(d->itemCollator.sortKey(as));
    }

    if (!d->itemSortKeys.contains(b))
    {
        QString bs = b;
        bs.replace(QLatin1String("_v"),
                   QLatin1String("vv"), Qt::CaseInsensitive);

        d->itemSortKeys[b].emplace_back(d->itemCollator.sortKey(bs));
    }

    return (d->itemSortKeys[a].front().compare(d->itemSortKeys[b].front()));
}

int ItemSorterCache::albumCompare(const QString& a, const QString& b,
                                  Qt::CaseSensitivity caseSensitive, bool natural) const
{
    if ((d->albumCollator.numericMode()     != natural)     ||
        (d->albumCollator.caseSensitivity() != caseSensitive))
    {
        d->albumSortKeys.clear();

        d->albumCollator.setNumericMode(natural);
        d->albumCollator.setCaseSensitivity(caseSensitive);
    }

    if (!d->albumSortKeys.contains(a))
    {
        d->albumSortKeys[a].emplace_back(d->albumCollator.sortKey(a));
    }

    if (!d->albumSortKeys.contains(b))
    {
        d->albumSortKeys[b].emplace_back(d->albumCollator.sortKey(b));
    }

    return (d->albumSortKeys[a].front().compare(d->albumSortKeys[b].front()));
}

} // namespace Digikam
