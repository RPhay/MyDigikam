/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "KmlLatLonQuadWriter.h"

#include "GeoDataLatLonQuad.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLatLonQuad(
    GeoTagWriter::QualifiedName( QString::fromUtf8(GeoDataTypes::GeoDataLatLonQuadType),
                                 QString::fromUtf8(kml::kmlTag_nameSpaceOgc22) ),
    new KmlLatLonQuadWriter );

bool KmlLatLonQuadWriter::write( const GeoNode *node,
                 GeoWriter& writer ) const
{
    const GeoDataLatLonQuad *latLonQuad = static_cast<const GeoDataLatLonQuad*>( node );

    if ( latLonQuad->isValid() ) {
        writer.writeStartElement( QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_LatLonQuad) );
        KmlObjectTagWriter::writeIdentifiers( writer, latLonQuad );

        writer.writeStartElement( QString::fromUtf8(kml::kmlTag_coordinates) );

        writer.writeCharacters( QString::number( latLonQuad->bottomLeftLongitude( GeoDataCoordinates::Degree ) ) );
        writer.writeCharacters( QString::fromUtf8( "," ) );
        writer.writeCharacters( QString::number( latLonQuad->bottomLeftLatitude( GeoDataCoordinates::Degree ) ) );
        writer.writeCharacters( QString::fromUtf8( " " ) );

        writer.writeCharacters( QString::number( latLonQuad->bottomRightLongitude( GeoDataCoordinates::Degree ) ) );
        writer.writeCharacters( QString::fromUtf8( "," ) );
        writer.writeCharacters( QString::number( latLonQuad->bottomRightLatitude( GeoDataCoordinates::Degree ) ) );
        writer.writeCharacters( QString::fromUtf8( " " ) );

        writer.writeCharacters( QString::number( latLonQuad->topRightLongitude( GeoDataCoordinates::Degree ) ) );
        writer.writeCharacters( QString::fromUtf8( "," ) );
        writer.writeCharacters( QString::number( latLonQuad->topRightLatitude( GeoDataCoordinates::Degree ) ) );
        writer.writeCharacters( QString::fromUtf8( " " ) );

        writer.writeCharacters( QString::number( latLonQuad->topLeftLongitude( GeoDataCoordinates::Degree ) ) );
        writer.writeCharacters( QString::fromUtf8( "," ) );
        writer.writeCharacters( QString::number( latLonQuad->topLeftLatitude( GeoDataCoordinates::Degree ) ) );

        writer.writeEndElement();

        writer.writeEndElement();
    }

    return true;
}

} // namespace Marble
