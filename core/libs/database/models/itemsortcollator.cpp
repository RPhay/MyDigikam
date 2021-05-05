/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-10-11
 * Description : item sort based on QCollator
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

#include "itemsortcollator.h"

// Qt includes

#include <QCollator>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemSortCollator::Private
{
public:

    explicit Private()
    {
    }

    QCollator itemCollator;
    QCollator albumCollator;
};

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN ItemSortCollatorCreator
{
public:

    ItemSortCollator object;
};

Q_GLOBAL_STATIC(ItemSortCollatorCreator, itemSortCollatorCreator)

// -----------------------------------------------------------------------------------------------

ItemSortCollator::ItemSortCollator()
    : d(new Private)
{
}

ItemSortCollator::~ItemSortCollator()
{
    delete d;
}

ItemSortCollator* ItemSortCollator::instance()
{
    return &itemSortCollatorCreator->object;
}

int ItemSortCollator::itemCompare(const QString& a, const QString& b,
                                  Qt::CaseSensitivity caseSensitive, bool natural) const
{
    d->itemCollator.setNumericMode(natural);
    d->itemCollator.setCaseSensitivity(caseSensitive);
    d->itemCollator.setIgnorePunctuation(a.contains(QLatin1String("_v"),
                                                    Qt::CaseInsensitive));

    return d->itemCollator.compare(a, b);
}

int ItemSortCollator::albumCompare(const QString& a, const QString& b,
                                   Qt::CaseSensitivity caseSensitive, bool natural) const
{
    d->albumCollator.setNumericMode(natural);
    d->albumCollator.setIgnorePunctuation(false);
    d->albumCollator.setCaseSensitivity(caseSensitive);

    return d->albumCollator.compare(a, b);
}

} // namespace Digikam
