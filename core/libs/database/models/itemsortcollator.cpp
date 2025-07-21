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
        itemCollator.setIgnorePunctuation(false);
        albumCollator.setIgnorePunctuation(false);
    }

public:

    const QString            versionStr     = QLatin1String("_v");
    const QRegularExpression versionExp     = QRegularExpression(QRegularExpression::anchoredPattern(QLatin1String("(.+)_v(\\d+)(\\..+)?")));

    QCollator                itemCollator;
    QCollator                albumCollator;
};

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN ItemSortCollatorCreator
{
public:

    ItemSortCollator object;
};

Q_GLOBAL_STATIC(ItemSortCollatorCreator, creator)

// -----------------------------------------------------------------------------------------------

ItemSortCollator::ItemSortCollator()
    : QObject(),
      d      (new Private)
{
}

ItemSortCollator::~ItemSortCollator()
{
    delete d;

    qDebug() << "ItemSortCollator::delete";
}

ItemSortCollator* ItemSortCollator::instance()
{
    return &creator->object;
}

int ItemSortCollator::itemCompare(const QString& a,
                                  const QString& b,
                                  Qt::CaseSensitivity caseSensitive,
                                  bool natural) const
{
    if (natural)
    {
        // Check if version string is included, this is
        // faster than always using QRegularExpression.

        d->itemCollator.setCaseSensitivity(caseSensitive);

        if (a.contains(d->versionStr) || b.contains(d->versionStr))
        {
            QString aa = a;
            QString bb = b;

            QRegularExpressionMatch aMatch = d->versionExp.match(a);
            QRegularExpressionMatch bMatch = d->versionExp.match(b);

            if (aMatch.hasMatch())
            {
                aa = aMatch.captured(1) + QLatin1String("@@") +
                     aMatch.captured(2) + aMatch.captured(3);
            }

            if (bMatch.hasMatch())
            {
                bb = bMatch.captured(1) + QLatin1String("@@") +
                     bMatch.captured(2) + bMatch.captured(3);
            }

            return d->itemCollator.compare(aa, bb);
        }

        return d->itemCollator.compare(a, b);
    }

    return QString::compare(a, b, caseSensitive);
}

int ItemSortCollator::albumCompare(const QString& a,
                                   const QString& b,
                                   Qt::CaseSensitivity caseSensitive,
                                   bool natural) const
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
