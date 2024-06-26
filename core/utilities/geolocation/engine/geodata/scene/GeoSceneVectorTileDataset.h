/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include "GeoSceneTileDataset.h"

namespace Marble
{

class GeoSceneVectorTileDataset : public GeoSceneTileDataset
{
 public:

    explicit GeoSceneVectorTileDataset( const QString& name );

    const char* nodeType() const override;
};

} // namespace Marble
