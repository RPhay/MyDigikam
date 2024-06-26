// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "KmlFeatureTagWriter.h"

class QString;

namespace Marble
{

// No registration for this writer, ColorStyle is an abstract kml element
class KmlOverlayTagWriter: public KmlFeatureTagWriter
{
public:
    explicit KmlOverlayTagWriter( const QString &elementName );

protected:
    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override = 0;
};

}
