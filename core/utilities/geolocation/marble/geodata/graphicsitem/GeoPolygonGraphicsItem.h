// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#pragma once

#include "AbstractGeoPolygonGraphicsItem.h"

namespace Marble
{

class GeoDataLinearRing;
class GeoDataPolygon;
class GeoDataBuilding;

class DIGIKAM_EXPORT GeoPolygonGraphicsItem : public AbstractGeoPolygonGraphicsItem
{
public:
    static AbstractGeoPolygonGraphicsItem *createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon);
    static AbstractGeoPolygonGraphicsItem *createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring);
    static AbstractGeoPolygonGraphicsItem *createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataBuilding *building);

    explicit GeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon);
    explicit GeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring);
};

} // namespace Marble
