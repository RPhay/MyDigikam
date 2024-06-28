// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#pragma once

#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlTourTagWriter : public KmlFeatureTagWriter
{
public:

    KmlTourTagWriter();

    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;
};

} // namespace Marble
