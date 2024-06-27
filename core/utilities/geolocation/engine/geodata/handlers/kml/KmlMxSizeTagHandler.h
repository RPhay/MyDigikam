// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

namespace mx
{

class KmlsizeTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser&) const override;
};

}

} // namespace Marble

}

