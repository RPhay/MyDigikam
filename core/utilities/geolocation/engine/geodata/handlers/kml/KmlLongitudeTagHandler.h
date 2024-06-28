// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmllongitudeTagHandler : public GeoTagHandler
{
public:

    GeoNode * parse ( GeoParser & ) const override;
};

} // namespace kml

} // namespace Marble
