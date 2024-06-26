// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace dgml
{

class DgmlTileSizeTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse( GeoParser& ) const override;
};

}

}
