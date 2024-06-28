// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyalovelyx@gmail.com>
//

#pragma once

#include "GeoTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlTimeSpanWriter : public GeoTagWriter
{
public:

    bool write( const GeoNode *node, GeoWriter& writer ) const override;

};

} // namespace Marble
