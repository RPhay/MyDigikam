// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlLodTagHandler : public GeoTagHandler
{
 public:
    GeoNode* parse( GeoParser& ) const override;
};

}

}
