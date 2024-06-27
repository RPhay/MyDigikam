// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#pragma once

#include "LayerInterface.h"

#include <QString>

namespace Marble
{

class MarbleSplashLayer : public LayerInterface
{
public:

    MarbleSplashLayer();

    QStringList renderPosition() const override;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString &renderPos = QLatin1String("SURFACE"),
                         GeoSceneLayer *layer = nullptr ) override;

    RenderState renderState() const override;

    QString runtimeTrace() const override { return QStringLiteral("SplashLayer"); }
};

} // namespace Marble
