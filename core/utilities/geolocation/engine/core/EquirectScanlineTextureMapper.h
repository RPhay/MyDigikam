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

#include "TextureMapperInterface.h"

#include "MarbleGlobal.h"

#include <QThreadPool>
#include <QImage>

namespace Marble
{

class StackedTileLoader;

class EquirectScanlineTextureMapper : public TextureMapperInterface
{
public:

    explicit EquirectScanlineTextureMapper( StackedTileLoader *tileLoader );

    void mapTexture( GeoPainter *painter,
                     const ViewportParams *viewport,
                     int tileZoomLevel,
                     const QRect &dirtyRect,
                     TextureColorizer *texColorizer ) override;

private:

    void mapTexture( const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality );

private:

    class RenderJob;

    StackedTileLoader* const m_tileLoader = nullptr;
    int                      m_radius;
    QImage                   m_canvasImage;
    int                      m_oldYPaintedTop;
    QThreadPool              m_threadPool;
};

} // Namespace Marble
