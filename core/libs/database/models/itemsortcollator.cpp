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

    Private()
    {
        itemCollator.setNumericMode(true);
        albumCollator.setNumericMode(true);
        albumCollator.setIgnorePunctuation(false);
    }

    const QString            versionStr     = QLatin1String("_v");
    const QRegularExpression versionExp     = QRegularExpression(QRegularExpression::anchoredPattern(QLatin1String("(.+)_v(\\d+)(.+)?")));

    QCollator                itemCollator;
    QCollator                albumCollator;
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
    if (natural)
    {
        bool hasVersion = false;

        // Check if version string is included, this is
        // faster than always using QRegularExpression.

        if (a.contains(d->versionStr) || b.contains(d->versionStr))
        {
            hasVersion = (d->versionExp.match(a).hasMatch() ||
                          d->versionExp.match(b).hasMatch());
        }

        d->itemCollator.setCaseSensitivity(caseSensitive);
        d->itemCollator.setIgnorePunctuation(hasVersion);

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

#include "moc_itemsortcollator.cpp"
