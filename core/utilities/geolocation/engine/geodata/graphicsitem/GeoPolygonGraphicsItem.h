/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "AbstractGeoPolygonGraphicsItem.h"

namespace Marble
{

class GeoDataLinearRing;
class GeoDataPolygon;
class GeoDataBuilding;

class DIGIKAM_EXPORT GeoPolygonGraphicsItem : public AbstractGeoPolygonGraphicsItem
{
public:

    static AbstractGeoPolygonGraphicsItem* createGraphicsItem(const GeoDataPlacemark* placemark, const GeoDataPolygon* polygon);
    static AbstractGeoPolygonGraphicsItem* createGraphicsItem(const GeoDataPlacemark* placemark, const GeoDataLinearRing* ring);
    static AbstractGeoPolygonGraphicsItem* createGraphicsItem(const GeoDataPlacemark* placemark, const GeoDataBuilding* building);

    explicit GeoPolygonGraphicsItem(const GeoDataPlacemark* placemark, const GeoDataPolygon* polygon);
    explicit GeoPolygonGraphicsItem(const GeoDataPlacemark* placemark, const GeoDataLinearRing* ring);
};

} // namespace Marble
