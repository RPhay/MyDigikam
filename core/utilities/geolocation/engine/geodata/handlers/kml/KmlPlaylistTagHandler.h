// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlPlaylistTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser &parser) const override;
};

} // namespace kml

} // namespace Marble
