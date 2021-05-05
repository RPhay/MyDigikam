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

#ifndef DIGIKAM_TEXTURE_CONTAINER_H
#define DIGIKAM_TEXTURE_CONTAINER_H

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT TextureContainer
{

public:

    enum TextureTypes
    {
        PaperTexture=0,
        Paper2Texture,
        FabricTexture,
        BurlapTexture,
        BricksTexture,
        Bricks2Texture,
        CanvasTexture,
        MarbleTexture,
        Marble2Texture,
        BlueJeanTexture,
        CellWoodTexture,
        MetalWireTexture,
        ModernTexture,
        WallTexture,
        MossTexture,
        StoneTexture
    };

public:

    explicit TextureContainer();
    ~TextureContainer();

    static QString getTexturePath(int texture);

public:

    int blendGain;
    int textureType;
};

} // namespace Digikam

#endif // DIGIKAM_TEXTURE_CONTAINER_H
