/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Data set for item lister
 *
 * SPDX-FileCopyrightText: 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2007-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_LISTER_RECORD_H
#define DIGIKAM_ITEM_LISTER_RECORD_H

// Qt includes

#include <QString>
#include <QDataStream>
#include <QDateTime>
#include <QSize>
#include <QList>
#include <QVariant>

// Local includes

#include "digikam_export.h"
#include "coredbconstants.h"
#include "coredbalbuminfo.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT ItemListerRecord
{

public:

    explicit ItemListerRecord();

    bool operator==(const ItemListerRecord& record) const;

public:

    int                    albumID;
    int                    albumRootID;
    int                    rating;

    qlonglong              fileSize;
    qlonglong              imageID;
    qlonglong              currentFuzzySearchReferenceImage;

    double                 currentSimilarity;

    QString                format;
    QString                name;

    QDateTime              creationDate;
    QDateTime              modificationDate;

    QSize                  imageSize;

    DatabaseItem::Category category;

    QList<QVariant>        extraValues;
};
/*
DIGIKAM_DATABASE_EXPORT QDataStream& operator<<(QDataStream& os, const ItemListerRecord& record);
DIGIKAM_DATABASE_EXPORT QDataStream& operator>>(QDataStream& ds, ItemListerRecord& record);
*/
} // namespace Digikam

#endif // DIGIKAM_ITEM_LISTER_RECORD_H
