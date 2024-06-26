// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlWaitTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser &parser) const override;
};

} // namespace kml

} // namespace Marble
