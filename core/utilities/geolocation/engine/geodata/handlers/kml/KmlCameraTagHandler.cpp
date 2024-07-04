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

#include "KmlCameraTagHandler.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataCamera.h"
#include "GeoParser.h"
#include "GeoDataFeature.h"
#include "GeoDataFlyTo.h"

#include "digikam_debug.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( Camera )

GeoNode *KmlCameraTagHandler::parse( GeoParser & parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Camera)));

    GeoDataCamera* camera = nullptr;
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataFeature>() ) {
        camera = new GeoDataCamera;
        KmlObjectTagHandler::parseIdentifiers( parser, camera );
        parentItem.nodeAs<GeoDataFeature>()->setAbstractView( camera );
    }

    if ( parentItem.is<GeoDataFlyTo>() ) {
        camera = new GeoDataCamera;
        KmlObjectTagHandler::parseIdentifiers( parser, camera );
        parentItem.nodeAs<GeoDataFlyTo>()->setView( camera );
    }

    return camera;
}

}

}
