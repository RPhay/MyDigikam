/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-16
 * Description : categorize item view based on DCategorizedView
 *
 * SPDX-FileCopyrightText: 2007      by Rafael Fernández López <ereslibre at kde dot org>
 * SPDX-FileCopyrightText: 2007      by John Tapsell <tapsell at kde dot org>
 * SPDX-FileCopyrightText: 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2011-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DCATEGORIZED_SORT_FILTER_PROXY_MODEL_PRIVATE_H
#define DIGIKAM_DCATEGORIZED_SORT_FILTER_PROXY_MODEL_PRIVATE_H

#include "dcategorizedsortfilterproxymodel.h"

// Qt includes

#include <QCollator>

namespace Digikam
{

class DCategorizedSortFilterProxyModel;

class Q_DECL_HIDDEN DCategorizedSortFilterProxyModel::Private
{
public:

    explicit Private()
      : sortColumn                       (0),
        sortOrder                        (Qt::AscendingOrder),
        categorizedModel                 (false),
        sortCategoriesByNaturalComparison(true)
    {
        collator.setNumericMode(true);
        collator.setCaseSensitivity(Qt::CaseSensitive);
    }

    ~Private()
    {
    }

public:

    int           sortColumn;
    Qt::SortOrder sortOrder;
    bool          categorizedModel;
    bool          sortCategoriesByNaturalComparison;
    QCollator     collator;
};

} // namespace Digikam

#endif // DIGIKAM_DCATEGORIZED_SORT_FILTER_PROXY_MODEL_PRIVATE_H
