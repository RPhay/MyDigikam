// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Anders Lund <anders@alweb.dk>
// SPDX-FileCopyrightText: 2009 Thibaut GRIDEL <tgridel@free.fr>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace gpx
{

class GPXdescTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}

}
