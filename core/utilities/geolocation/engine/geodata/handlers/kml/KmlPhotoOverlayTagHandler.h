// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlPhotoOverlayTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser&) const override;
};

}

} // namespace Marble
