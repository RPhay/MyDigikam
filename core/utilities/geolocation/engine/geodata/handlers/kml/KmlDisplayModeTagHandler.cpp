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

#include "KmlDisplayModeTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataBalloonStyle.h"
#include "GeoDataParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( displayMode )

GeoNode* KmldisplayModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_displayMode)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_BalloonStyle ) )
    {
        QString mode = parser.readElementText().trimmed();
        GeoDataBalloonStyle::DisplayMode displayMode = GeoDataBalloonStyle::Default;
        if (mode == QLatin1String("default")) {
                displayMode = GeoDataBalloonStyle::Default;
        } else if (mode == QLatin1String("hide")) {
                displayMode = GeoDataBalloonStyle::Hide;
        }

        parentItem.nodeAs<GeoDataBalloonStyle>()->setDisplayMode( displayMode );
    }
    return nullptr;
}

}
}
