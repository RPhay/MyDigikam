// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//


#include "KmlDataTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "GeoDataData.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerData( GeoTagWriter::QualifiedName( QString::fromUtf8(GeoDataTypes::GeoDataDataType),
                                                                            QString::fromUtf8(kml::kmlTag_nameSpaceOgc22) ),
                                               new KmlDataTagWriter() );


bool KmlDataTagWriter::write( const GeoNode *node,
                               GeoWriter& writer ) const
{
    const GeoDataData *data = static_cast<const GeoDataData*>( node );

    writer.writeStartElement( QString::fromUtf8(kml::kmlTag_Data) );
    writer.writeAttribute( QString::fromUtf8("name"), data->name() );
    writer.writeOptionalElement( QString::fromUtf8(kml::kmlTag_displayName), data->displayName() );
    writer.writeElement( QString::fromUtf8("value"), data->value().toString() );
    writer.writeEndElement();

    return true;
}

}
