// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#pragma once

#include "TextureMapperInterface.h"

#include <QThreadPool>
#include <QImage>

#include <MarbleGlobal.h>

namespace Marble
{

class StackedTileLoader;

class GenericScanlineTextureMapper : public TextureMapperInterface
{
 public:
    explicit GenericScanlineTextureMapper( StackedTileLoader *tileLoader );

    void mapTexture( GeoPainter *painter,
                             const ViewportParams *viewport,
                             int tileZoomLevel,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer ) override;

private:

    class RenderJob;

    void mapTexture( const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality );

private:

    StackedTileLoader* const m_tileLoader = nullptr;
    int m_radius;
    QImage m_canvasImage;
    QThreadPool m_threadPool;
};

} // Namespace Marble
