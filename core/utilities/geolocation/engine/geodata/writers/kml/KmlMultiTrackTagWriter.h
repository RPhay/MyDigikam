// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>
//

#pragma once

#include "GeoTagWriter.h"

namespace Marble
{

class KmlMultiTrackTagWriter : public GeoTagWriter
{
public:

    bool write( const GeoNode *node, GeoWriter& writer ) const override;
};

} // namespace Marble
