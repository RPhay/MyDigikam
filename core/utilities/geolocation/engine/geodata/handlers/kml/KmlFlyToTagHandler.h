// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlFlyToTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser&) const override;
};

} // namespace kml

} // namespace Marble
