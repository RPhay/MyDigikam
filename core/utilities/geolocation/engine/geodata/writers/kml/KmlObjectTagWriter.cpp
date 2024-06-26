// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlObjectTagWriter.h"

#include "GeoWriter.h"

namespace Marble {

void KmlObjectTagWriter::writeIdentifiers( GeoWriter &writer, const GeoDataObject *object )
{
    if ( object && !object->id().isEmpty() ) {
        writer.writeAttribute( QLatin1String("id"),
                               object->id() );
    }
    if ( object && !object->targetId().isEmpty() ) {
        writer.writeAttribute( QLatin1String("targetId"),
                               object->targetId() );
    }
}

}
