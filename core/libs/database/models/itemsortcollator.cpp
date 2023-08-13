/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-10-11
 * Description : item sort based on QCollator
 *
 * SPDX-FileCopyrightText: 2020 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemsortcollator.h"

// Qt includes

#include <QCollator>
#include <QRegularExpression>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemSortCollator::Private
{
public:

    explicit Private()
    {
        itemCollator.setNumericMode(true);
        albumCollator.setNumericMode(true);
        itemCollator.setIgnorePunctuation(false);
        albumCollator.setIgnorePunctuation(false);
        versionPattern.setPattern(QRegularExpression::anchoredPattern(QLatin1String("(.+?)_v(\\d+)(.*)?")));
    }

    QCollator          itemCollator;
    QCollator          albumCollator;
    QRegularExpression versionPattern;
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
    if (natural && !d->versionPattern.match(a).hasMatch())
    {
        d->itemCollator.setCaseSensitivity(caseSensitive);

        return d->itemCollator.compare(a, b);
    }

    return QString::compare(a, b, caseSensitive);
}

int ItemSortCollator::albumCompare(const QString& a, const QString& b,
                                   Qt::CaseSensitivity caseSensitive, bool natural) const
{
    if (natural)
    {
        d->albumCollator.setCaseSensitivity(caseSensitive);

        return d->albumCollator.compare(a, b);
    }

    return QString::compare(a, b, caseSensitive);
}

} // namespace Digikam
