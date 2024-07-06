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

#include "KmlOverlayTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoDataOverlay.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlColorStyleTagWriter.h"

namespace Marble
{

KmlOverlayTagWriter::KmlOverlayTagWriter(const QString &elementName)
    : KmlFeatureTagWriter( elementName )
{
    // nothing to do
}

bool KmlOverlayTagWriter::writeMid( const Marble::GeoNode *node, GeoWriter &writer ) const
{
    GeoDataOverlay const *overlay = static_cast<const GeoDataOverlay*>(node);

    QString const color = KmlColorStyleTagWriter::formatColor( overlay->color() );
    writer.writeOptionalElement( QString::fromUtf8(kml::kmlTag_color), color, QString::fromUtf8("ffffffff") );
    QString const drawOrder = QString::number( overlay->drawOrder() );
    writer.writeOptionalElement( QString::fromUtf8(kml::kmlTag_drawOrder), drawOrder, QString::fromUtf8("0") );

    if ( !overlay->iconFile().isEmpty() ) {
        writer.writeStartElement( QString::fromUtf8(kml::kmlTag_Icon) );
        writer.writeStartElement( QString::fromUtf8(kml::kmlTag_href) );
        writer.writeCharacters( overlay->iconFile() );
        writer.writeEndElement();
        writer.writeEndElement();
    }

    return true;
}

} // namespace Marble
