// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlLineStyleTagWriter.h"

#include "GeoDataLineStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLineStyle(
    GeoTagWriter::QualifiedName( QString::fromUtf8(GeoDataTypes::GeoDataLineStyleType),
                                 QString::fromUtf8(kml::kmlTag_nameSpaceOgc22) ),
    new KmlLineStyleTagWriter );

KmlLineStyleTagWriter::KmlLineStyleTagWriter() : KmlColorStyleTagWriter( QString::fromUtf8(kml::kmlTag_LineStyle) )
{
    // nothing to do
}

bool KmlLineStyleTagWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataLineStyle *style = static_cast<const GeoDataLineStyle*>( node );
    writer.writeOptionalElement( QString::fromUtf8("width"), style->width(), 1.0f );
    return true;
}

bool KmlLineStyleTagWriter::isEmpty( const GeoNode *node ) const
{
    const GeoDataLineStyle *style = static_cast<const GeoDataLineStyle*>( node );
    return style->width() == 1.0;
}

}
