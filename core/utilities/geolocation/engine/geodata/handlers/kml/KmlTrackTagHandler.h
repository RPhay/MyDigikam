// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlTrackTagHandler : public Marble::GeoTagHandler
{

public:
    GeoNode *parse( GeoParser &parser ) const override;
};

}

}
