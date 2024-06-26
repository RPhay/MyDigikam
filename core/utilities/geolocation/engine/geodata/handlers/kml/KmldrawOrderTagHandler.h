// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmldrawOrderTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser &parser) const override;
};

} // namespace kml

} // namespace Marble
