/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-01
 * Description : GPSSync common functions and structures
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2010      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_GPS_COMMON_H
#define DIGIKAM_GPS_COMMON_H

// Qt includes

#include <QString>
#include <QUrl>

// Local includes

#include "geocoordinates.h"
#include "digikam_export.h"

namespace Digikam
{

enum MapLayout
{
    MapLayoutOne        = 0,
    MapLayoutHorizontal = 1,
    MapLayoutVertical   = 2
};

DIGIKAM_EXPORT QString getUserAgentName();

void DIGIKAM_EXPORT coordinatesToClipboard(const GeoCoordinates& coordinates,
                                           const QUrl& url,
                                           const QString& title);

bool DIGIKAM_EXPORT checkSidecarSettings();

} // namespace Digikam

Q_DECLARE_METATYPE(Digikam::MapLayout)

#endif // DIGIKAM_GPS_COMMON_H
