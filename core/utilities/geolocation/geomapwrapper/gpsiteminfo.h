/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-01-06
 * Description : Helper class for geomap interaction
 *
 * SPDX-FileCopyrightText: 2011      by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2011-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_GPS_ITEM_INFO_H
#define DIGIKAM_GPS_ITEM_INFO_H

// Qt includes

#include <QDateTime>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "geocoordinates.h"
#include "geogroupstate.h"

namespace Digikam
{

class DIGIKAM_EXPORT GPSItemInfo
{
public:

    explicit GPSItemInfo();
    ~GPSItemInfo();

public:

    static GPSItemInfo fromIdCoordinatesRatingDateTime(const qlonglong p_id,
                                                       const GeoCoordinates& p_coordinates,
                                                       const int p_rating,
                                                       const QDateTime& p_creationDate);

public:

    qlonglong                  id;
    GeoCoordinates             coordinates;
    int                        rating;
    QDateTime                  dateTime;
    QUrl                       url;

    typedef QList<GPSItemInfo> List;
};

} // namespace Digikam

Q_DECLARE_METATYPE(Digikam::GPSItemInfo)

#endif // DIGIKAM_GPS_ITEM_INFO_H
