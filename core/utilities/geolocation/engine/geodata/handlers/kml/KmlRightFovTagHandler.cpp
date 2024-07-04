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

#include "KmlRightFovTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataViewVolume.h"
#include "GeoDataParser.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( rightFov )

GeoNode* KmlrightFovTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_rightFov)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ViewVolume ))
    {
        qreal rightFov = parser.readElementText().toDouble();

        parentItem.nodeAs<GeoDataViewVolume>()->setRightFov( rightFov );
    }
    return nullptr;
}

}
}
