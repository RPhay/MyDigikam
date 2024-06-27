// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
// SPDX-FileCopyrightText: 2010 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

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
