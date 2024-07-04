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

#include "KmlTagTagHandler.h"

#include "KmlElementDictionary.h"
#include "OsmPlacemarkData.h"
#include "GeoParser.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER_MX( tag )

GeoNode* KmltagTagHandler::parse( GeoParser& parser ) const
{
    if ( parser.parentElement().is<OsmPlacemarkData>() ) {
        QString key = parser.attribute( "k" );
        QString value = parser.attribute( "v" );
        OsmPlacemarkData *osmData = parser.parentElement().nodeAs<OsmPlacemarkData>();
        osmData->addTag( key, value );
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
