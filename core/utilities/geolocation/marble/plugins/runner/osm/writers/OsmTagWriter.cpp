// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#include "OsmTagWriter.h"

#include "GeoWriter.h"
#include "OsmElementDictionary.h"
#include "MarbleGlobal.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerOsm( GeoTagWriter::QualifiedName( QString::fromUtf8(""),
                                                                       QString::fromUtf8(osm::osmTag_version06) ),
                                               new OsmTagWriter() );


bool OsmTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    Q_UNUSED( node );

    writer.writeStartElement( QLatin1String("osm") );
    writer.writeAttribute(QLatin1String("version"), QString::fromUtf8(osm::osmTag_version06) );
    writer.writeAttribute(QLatin1String("generator"), QLatin1String("Marble ") + Marble::MARBLE_VERSION_STRING);

    return true;
}

}

