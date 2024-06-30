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

#include <QString>

#include "LayerInterface.h"

namespace Marble
{

class FogLayer : public LayerInterface
{
public:

    QStringList renderPosition() const override;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = QLatin1String("NONE"),
                         GeoSceneLayer * layer = nullptr ) override;

    RenderState renderState() const override;
    QString runtimeTrace() const override { return QStringLiteral("FogLayer"); }
};

} // namespace Marble
