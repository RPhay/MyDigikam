// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#pragma once

#include "GeoTagWriter.h"
#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlFolderTagWriter : public KmlFeatureTagWriter
{
public:

    KmlFolderTagWriter();

protected:

    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;
};

} // namespace Marble
