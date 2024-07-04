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

#include "KmlRotationTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( rotation )

GeoNode* KmlrotationTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_rotation)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ScreenOverlay ))
    {
        qreal rotation = parser.readElementText().toFloat();

        parentItem.nodeAs<GeoDataScreenOverlay>()->setRotation( rotation );
    }
    else if (parentItem.represents( kmlTag_LatLonBox ))
    {
        qreal rotation = parser.readElementText().toFloat();

        parentItem.nodeAs<GeoDataLatLonBox>()->setRotation( rotation * DEG2RAD );
    }
    else if (parentItem.represents( kmlTag_PhotoOverlay ))
    {
        qreal rotation = parser.readElementText().toFloat();

        parentItem.nodeAs<GeoDataPhotoOverlay>()->setRotation( rotation );
    }
    return nullptr;
}

}
}
