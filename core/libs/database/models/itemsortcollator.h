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

#ifndef DIGIKAM_ITEM_SORT_COLLATOR_H
#define DIGIKAM_ITEM_SORT_COLLATOR_H

// Qt includes

#include <QObject>
#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT ItemSortCollator : public QObject
{
    Q_OBJECT

public:

    /**
     * Global instance of internal item sort collator.
     * All accessor methods are thread-safe.
     */
    static ItemSortCollator* instance();

    int itemCompare(const QString& a, const QString& b,
                    Qt::CaseSensitivity caseSensitive, bool natural)  const;

    int albumCompare(const QString& a, const QString& b,
                     Qt::CaseSensitivity caseSensitive, bool natural) const;

private:

    // Disable
    ItemSortCollator();
    explicit ItemSortCollator(QObject*) = delete;
    ~ItemSortCollator() override;

private:

    class Private;
    Private* const d;

    friend class ItemSortCollatorCreator;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_SORT_COLLATOR_H
