// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#include "OsmObjectAttributeWriter.h"
#include "OsmObjectManager.h"
#include "OsmPlacemarkData.h"
#include "GeoWriter.h"

namespace Marble {

void OsmObjectAttributeWriter::writeAttributes( const OsmPlacemarkData& osmData, GeoWriter &writer )
{
    writer.writeAttribute( QLatin1String("id"), QString::number( osmData.id() ) );
    writer.writeOptionalAttribute( QLatin1String("action"), osmData.action() );
    writer.writeOptionalAttribute( QLatin1String("changeset"), osmData.changeset() );
    writer.writeOptionalAttribute( QLatin1String("timestamp"), osmData.timestamp() );
    writer.writeOptionalAttribute( QLatin1String("uid"), osmData.uid() );
    writer.writeOptionalAttribute( QLatin1String("user"), osmData.user() );
    writer.writeOptionalAttribute( QLatin1String("version"), osmData.version() );
    writer.writeOptionalAttribute( QLatin1String("visible"), osmData.isVisible() );
}

}

