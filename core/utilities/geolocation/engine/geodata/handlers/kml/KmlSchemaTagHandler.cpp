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

#include "KmlSchemaTagHandler.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataDocument.h"
#include "GeoDataSchema.h"
#include "GeoParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( Schema )

GeoNode* KmlSchemaTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Schema)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Document ) ) {
        GeoDataSchema schema;
        KmlObjectTagHandler::parseIdentifiers( parser, &schema );
        QString name = parser.attribute( "name" ).trimmed();

        schema.setSchemaName( name );
        parentItem.nodeAs<GeoDataDocument>()->addSchema( schema );
        return &parentItem.nodeAs<GeoDataDocument>()->schema( schema.id() );
    }
    return nullptr;

}

} // namespace kml

} // namespace Marble
