// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#pragma once

#include "GeoDataGeometry_p.h"

namespace Marble
{

class GeoDataPointPrivate : public GeoDataGeometryPrivate
{
public:

    GeoDataCoordinates m_coordinates;

    GeoDataPointPrivate()
    {
    }

    GeoDataGeometryPrivate *copy() const override
    {
        GeoDataPointPrivate* copy = new  GeoDataPointPrivate;
        *copy = *this;
        return copy;
    }
};

} // namespace Marble
