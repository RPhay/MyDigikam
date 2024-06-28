// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#pragma once

#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlPlacemarkTagWriter : public KmlFeatureTagWriter
{
public:

  KmlPlacemarkTagWriter();

protected:

    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;
};

} // namespace Marble
