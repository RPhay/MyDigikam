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

#include "KmlBalloonVisibilityTagHandler.h"

#include "GeoDataPlacemark.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

#include "digikam_debug.h"

namespace Marble
{
namespace kml
{
namespace gx
{
KML_DEFINE_TAG_HANDLER_GX22( balloonVisibility )

GeoNode* KmlballoonVisibilityTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_balloonVisibility)));

    QString content = parser.readElementText().trimmed();

    const bool visible = (content == QLatin1String("1"));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.is<GeoDataPlacemark>() ){
        parentItem.nodeAs<GeoDataPlacemark>()->setBalloonVisible( visible );
    }

    return nullptr;
}

}
}
}
