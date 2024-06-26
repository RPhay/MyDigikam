// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#pragma once

#include "GeoTagWriter.h"

namespace Marble {

class KmlTrackWriter : public GeoTagWriter
{

public:
    bool write( const GeoNode *node, GeoWriter &writer ) const override;
};

}
