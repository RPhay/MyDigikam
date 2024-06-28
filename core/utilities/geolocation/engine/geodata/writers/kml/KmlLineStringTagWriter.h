// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "GeoTagWriter.h"

namespace Marble
{

class KmlLineStringTagWriter : public GeoTagWriter
{
public:

    bool write( const GeoNode *node, GeoWriter& writer ) const override;
};

} // namespace Marble
