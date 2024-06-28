// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Levente Kurusa <levex@linux.com>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlSnippetTagHandler : public GeoTagHandler
{
public:

    GeoNode * parse( GeoParser & ) const override;
};

} // namespace kml

} // namespace Marble
