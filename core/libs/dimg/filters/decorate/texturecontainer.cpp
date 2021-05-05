/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : texture settings container.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "texturecontainer.h"

// Qt includes

#include <QStandardPaths>

namespace Digikam
{

TextureContainer::TextureContainer()
    : blendGain  (200),
      textureType(MarbleTexture)
{
}

TextureContainer::~TextureContainer()
{
}

QString TextureContainer::getTexturePath(int texture)
{
    QString pattern;

    switch (texture)
    {
        case PaperTexture:
            pattern = QLatin1String("paper-texture");
            break;

        case Paper2Texture:
            pattern = QLatin1String("paper2-texture");
            break;

        case FabricTexture:
            pattern = QLatin1String("fabric-texture");
            break;

        case BurlapTexture:
            pattern = QLatin1String("burlap-texture");
            break;

        case BricksTexture:
            pattern = QLatin1String("bricks-texture");
            break;

        case Bricks2Texture:
            pattern = QLatin1String("bricks2-texture");
            break;

        case CanvasTexture:
            pattern = QLatin1String("canvas-texture");
            break;

        case MarbleTexture:
            pattern = QLatin1String("marble-texture");
            break;

        case Marble2Texture:
            pattern = QLatin1String("marble2-texture");
            break;

        case BlueJeanTexture:
            pattern = QLatin1String("bluejean-texture");
            break;

        case CellWoodTexture:
            pattern = QLatin1String("cellwood-texture");
            break;

        case MetalWireTexture:
            pattern = QLatin1String("metalwire-texture");
            break;

        case ModernTexture:
            pattern = QLatin1String("modern-texture");
            break;

        case WallTexture:
            pattern = QLatin1String("wall-texture");
            break;

        case MossTexture:
            pattern = QLatin1String("moss-texture");
            break;

        case StoneTexture:
            pattern = QLatin1String("stone-texture");
            break;
    }

    return (QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/") + pattern + QLatin1String(".png")));
}

} // namespace Digikam
