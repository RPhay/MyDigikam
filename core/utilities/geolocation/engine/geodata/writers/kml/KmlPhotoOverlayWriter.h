// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyalovelyx@gmail.com>
//

#pragma once

#include "KmlOverlayTagWriter.h"

namespace Marble
{

class KmlPhotoOverlayWriter : public KmlOverlayTagWriter
{
public:

    KmlPhotoOverlayWriter();

    bool writeMid( const GeoNode *node, GeoWriter &writer ) const override;
};

} // namespace Marble
