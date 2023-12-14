// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyatf@gmail.com>
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlGroundOverlayWriter.h"

#include "GeoDataLatLonQuad.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( QString::fromUtf8(GeoDataTypes::GeoDataGroundOverlayType),
                 QString::fromUtf8(kml::kmlTag_nameSpaceOgc22) ),
    new KmlGroundOverlayWriter );

KmlGroundOverlayWriter::KmlGroundOverlayWriter() : KmlOverlayTagWriter( QString::fromUtf8(kml::kmlTag_GroundOverlay) )
{
    // nothing to do
}

bool KmlGroundOverlayWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    KmlOverlayTagWriter::writeMid( node, writer );

    const GeoDataGroundOverlay *ground_overlay =
        static_cast<const GeoDataGroundOverlay*>( node );

    writer.writeOptionalElement( QString::fromUtf8(kml::kmlTag_altitude),
                                 QString::number(ground_overlay->altitude()), QString::fromUtf8("0") );
    KmlGroundOverlayWriter::writeAltitudeMode( writer, ground_overlay->altitudeMode() );

    if ( !ground_overlay->latLonBox().isEmpty() ) {
        writeElement( &ground_overlay->latLonBox(), writer );
    }

    if ( ground_overlay->latLonQuad().isValid() ) {
        writeElement( &ground_overlay->latLonQuad(), writer );
    }

    return true;
}

QString KmlGroundOverlayWriter::altitudeModeToString(AltitudeMode mode)
{
    switch (mode) {
    case ClampToGround:
    return QString::fromUtf8("clampToGround");
    case RelativeToGround:
    return QString::fromUtf8("relativeToGround");
    case ClampToSeaFloor:
    return QString::fromUtf8("clampToSeaFloor");
    case RelativeToSeaFloor:
    return QString::fromUtf8("relativeToSeaFloor");
    case Absolute:
    return QString::fromUtf8("absolute");
    }
    return QString::fromUtf8("");
}

void KmlGroundOverlayWriter::writeAltitudeMode(GeoWriter& writer, AltitudeMode altMode)
{
    if ( altMode == ClampToGround ) {
        // clampToGround is always the default value, so we never have to write it
        return;
    }

    const QString altitudeMode = KmlGroundOverlayWriter::altitudeModeToString( altMode );
    bool const isGoogleExtension = ( altMode == ClampToSeaFloor || altMode == RelativeToSeaFloor );
    if ( isGoogleExtension ) {
        // clampToSeaFloor and relativeToSeaFloor are Google extensions that need a gx: tag namespace
        writer.writeElement( QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_altitudeMode), altitudeMode);
    } else {
        writer.writeElement( QString::fromUtf8(kml::kmlTag_altitudeMode), altitudeMode );
    }
}

}

