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

#include "KmlStyleMapTagHandler.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataFeature.h"
#include "GeoDataDocument.h"
#include "GeoDataStyleMap.h"
#include "GeoParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( StyleMap )

GeoNode* KmlStyleMapTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_StyleMap)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Document ) ) {
        GeoDataStyleMap styleMap;
        KmlObjectTagHandler::parseIdentifiers( parser, &styleMap );
        parentItem.nodeAs<GeoDataDocument>()->addStyleMap( styleMap );
        return &parentItem.nodeAs<GeoDataDocument>()->styleMap( styleMap.id() );
    } else if( parentItem.is<GeoDataFeature>() ) {
/*        GeoDataStyleMap styleMap;
        parentItem.nodeAs<GeoDataFeature>()->setStyleMap( styleMap );*/
    }

    return nullptr;
}

}
}
