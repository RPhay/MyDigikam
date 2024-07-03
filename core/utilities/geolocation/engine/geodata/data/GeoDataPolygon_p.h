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

#include "GeoDataGeometry_p.h"

#include "GeoDataLinearRing.h"

namespace Marble
{

class GeoDataPolygonPrivate : public GeoDataGeometryPrivate
{
public:

    explicit GeoDataPolygonPrivate( TessellationFlags f )
        : m_dirtyBox( true ),
          m_tessellationFlags(f),
          m_renderOrder(0)
    {
    }

    GeoDataPolygonPrivate()
        : m_dirtyBox( true )
    {
    }

    GeoDataGeometryPrivate *copy() const override
    {
        GeoDataPolygonPrivate* copy = new  GeoDataPolygonPrivate;
        *copy = *this;
        return copy;
    }

public:

    GeoDataLinearRing           outer;
    QVector<GeoDataLinearRing>  inner;
    bool                        m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has
                                            // been calculated. Saves performance.
    TessellationFlags           m_tessellationFlags;
    int                         m_renderOrder;
};

} // namespace Marble
