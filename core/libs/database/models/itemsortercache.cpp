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
      : albumSensitive(Qt::CaseSensitive),
        itemSensitive (Qt::CaseSensitive),
        albumNatural  (true),
        itemNatural   (true)
    {
    }

    QHash<QString, std::vector<QCollatorSortKey> > albumSortKeys;
    QHash<QString, std::vector<QCollatorSortKey> > itemSortKeys;

    Qt::CaseSensitivity                            albumSensitive;
    Qt::CaseSensitivity                            itemSensitive;

    bool                                           albumNatural;
    bool                                           itemNatural;
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
    if ((d->albumSensitive != caseSensitive) || (d->albumNatural != natural))
    {
        d->albumSensitive = caseSensitive;
        d->albumNatural   = natural;
        d->albumSortKeys.clear();
    }

    const bool containsA = d->albumSortKeys.contains(a);
    const bool containsB = d->albumSortKeys.contains(b);

    if (!containsA || !containsB)
    {
        QCollator collator;
        collator.setIgnorePunctuation(false);
        collator.setNumericMode(d->albumNatural);
        collator.setCaseSensitivity(d->albumSensitive);

        if (!containsA)
        {
            d->albumSortKeys[a].emplace_back(collator.sortKey(a));
        }

        if (!containsB)
        {
            d->albumSortKeys[b].emplace_back(collator.sortKey(b));
        }
    }

    return (d->albumSortKeys[a].front().compare(d->albumSortKeys[b].front()));
}

int ItemSorterCache::itemCompare(const QString& a, const QString& b,
                                 Qt::CaseSensitivity caseSensitive, bool natural) const
{
    if ((d->itemSensitive != caseSensitive) || (d->itemNatural != natural))
    {
        d->itemSensitive = caseSensitive;
        d->itemNatural   = natural;
        d->itemSortKeys.clear();
    }

    const bool containsA = d->itemSortKeys.contains(a);
    const bool containsB = d->itemSortKeys.contains(b);

    if (!containsA || !containsB)
    {
        QCollator collator;
        collator.setIgnorePunctuation(false);
        collator.setNumericMode(d->itemNatural);
        collator.setCaseSensitivity(d->itemSensitive);

        if (!containsA)
        {
            QString as = a;
            as.replace(QLatin1String("_v"),
                       QLatin1String("vv"), Qt::CaseInsensitive);

            d->itemSortKeys[a].emplace_back(collator.sortKey(as));
        }

        if (!containsB)
        {
            QString bs = b;
            bs.replace(QLatin1String("_v"),
                       QLatin1String("vv"), Qt::CaseInsensitive);

            d->itemSortKeys[b].emplace_back(collator.sortKey(bs));
        }
    }

    return (d->itemSortKeys[a].front().compare(d->itemSortKeys[b].front()));
}

} // namespace Digikam
