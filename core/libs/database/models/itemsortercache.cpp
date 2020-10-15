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

    QHash<QString, std::vector<QCollatorSortKey> > albumSortKeys;
    QHash<QString, std::vector<QCollatorSortKey> > itemSortKeys;

    QCollator                                      albumCollator;
    QCollator                                      itemCollator;
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
    d->albumCollator.setNumericMode(true);
    d->albumCollator.setIgnorePunctuation(false);
    d->albumCollator.setCaseSensitivity(Qt::CaseSensitive);

    d->itemCollator.setNumericMode(true);
    d->itemCollator.setIgnorePunctuation(false);
    d->itemCollator.setCaseSensitivity(Qt::CaseSensitive);
}

ItemSorterCache::~ItemSorterCache()
{
    delete d;
}

ItemSorterCache* ItemSorterCache::instance()
{
    return &itemSorterCacheCreator->object;
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

    const bool containsA = d->albumSortKeys.contains(a);
    const bool containsB = d->albumSortKeys.contains(b);

    if (!containsA || !containsB)
    {
        if (!containsA)
        {
            d->albumSortKeys[a].emplace_back(d->albumCollator.sortKey(a));
        }

        if (!containsB)
        {
            d->albumSortKeys[b].emplace_back(d->albumCollator.sortKey(b));
        }
    }

    return (d->albumSortKeys[a].front().compare(d->albumSortKeys[b].front()));
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

    const bool containsA = d->itemSortKeys.contains(a);
    const bool containsB = d->itemSortKeys.contains(b);

    if (!containsA || !containsB)
    {
        if (!containsA)
        {
            QString as = a;
            as.replace(QLatin1String("_v"),
                       QLatin1String("vv"), Qt::CaseInsensitive);

            d->itemSortKeys[a].emplace_back(d->itemCollator.sortKey(as));
        }

        if (!containsB)
        {
            QString bs = b;
            bs.replace(QLatin1String("_v"),
                       QLatin1String("vv"), Qt::CaseInsensitive);

            d->itemSortKeys[b].emplace_back(d->itemCollator.sortKey(bs));
        }
    }

    return (d->itemSortKeys[a].front().compare(d->itemSortKeys[b].front()));
}

} // namespace Digikam
