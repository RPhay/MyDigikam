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

#include "GeoSceneAbstractTileProjection.h"

#include "GeoDataLatLonBox.h"
#include "TileId.h"

namespace Marble
{

class GeoSceneAbstractTileProjectionPrivate
{
public:

    GeoSceneAbstractTileProjectionPrivate();

public:

    int levelZeroColumns;
    int levelZeroRows;
};

GeoSceneAbstractTileProjectionPrivate::GeoSceneAbstractTileProjectionPrivate()
    : levelZeroColumns(1)
    , levelZeroRows(1)
{
}

GeoSceneAbstractTileProjection::GeoSceneAbstractTileProjection()
    : d_ptr(new GeoSceneAbstractTileProjectionPrivate())
{
}

GeoSceneAbstractTileProjection::~GeoSceneAbstractTileProjection()
{
}

int GeoSceneAbstractTileProjection::levelZeroColumns() const
{
    return d_ptr->levelZeroColumns;
}

void GeoSceneAbstractTileProjection::setLevelZeroColumns(int levelZeroColumns)
{
    d_ptr->levelZeroColumns = levelZeroColumns;
}

int GeoSceneAbstractTileProjection::levelZeroRows() const
{
    return d_ptr->levelZeroRows;
}

void GeoSceneAbstractTileProjection::setLevelZeroRows(int levelZeroRows)
{
    d_ptr->levelZeroRows = levelZeroRows;
}

GeoDataLatLonBox GeoSceneAbstractTileProjection::geoCoordinates(const TileId &tileId) const
{
    return geoCoordinates(tileId.zoomLevel(), tileId.x(), tileId.y());
}

} // namespace Marble
