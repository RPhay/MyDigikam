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

#include "KmlGroundOverlayTagHandler.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( GroundOverlay )

GeoNode* KmlGroundOverlayTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_GroundOverlay)));

    GeoDataGroundOverlay *overlay = new GeoDataGroundOverlay;
    KmlObjectTagHandler::parseIdentifiers( parser, overlay );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ||
        parentItem.represents( kmlTag_Change ) || parentItem.represents( kmlTag_Create ) || parentItem.represents( kmlTag_Delete ) ) {
        parentItem.nodeAs<GeoDataContainer>()->append( overlay );
        return overlay;
    } else if (parentItem.qualifiedName().first == QLatin1String(kmlTag_kml)) {
        GeoDataDocument* doc = geoDataDoc( parser );
        doc->append( overlay );
        return overlay;
    } else {
        delete overlay;
        return nullptr;
    }
}

}
}
