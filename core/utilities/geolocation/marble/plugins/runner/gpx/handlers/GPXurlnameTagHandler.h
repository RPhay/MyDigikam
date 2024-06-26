// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Anders Lund <anders@alweb.dk>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace gpx
{

/// text ooccurs only in link in gpx 1.1
class GPXurlnameTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}

}
