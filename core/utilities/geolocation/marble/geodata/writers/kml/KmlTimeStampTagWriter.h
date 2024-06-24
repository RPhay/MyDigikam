// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Javier Becerra <javier@auva.es>
//

#pragma once

#include "GeoTagWriter.h"

namespace Marble
{
class GeoDataTimeStamp;

class KmlTimeStampTagWriter : public GeoTagWriter
{
public:
    bool write( const GeoNode *node, GeoWriter& writer ) const override;

    static QString toString( const GeoDataTimeStamp &timestamp );
};

}
