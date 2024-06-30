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

#include <LayerInterface.h>
#include <QObject>
#include "GeoSceneAbstractTileProjection.h"

namespace Marble
{

class DIGIKAM_EXPORT TileLayer :  public QObject, public LayerInterface
{
    Q_OBJECT

public:

    TileLayer();

    QStringList renderPosition() const override;

    virtual QSize tileSize() const = 0;
    virtual const GeoSceneAbstractTileProjection *tileProjection() const = 0;

    virtual int tileColumnCount( int level ) const = 0;
    virtual int tileRowCount( int level ) const = 0;

    virtual int layerCount() const = 0;
};

} // namespace Marble
