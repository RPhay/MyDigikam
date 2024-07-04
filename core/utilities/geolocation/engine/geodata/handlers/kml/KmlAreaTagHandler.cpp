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

#include "KmlAreaTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( area )

GeoNode* KmlareaTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_area)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.is<GeoDataPlacemark>() ) {
        QString area = parser.readElementText().trimmed();
        qreal ar = area.toDouble();
        if( ar < 0.0 ) ar = 0.0;
        parentItem.nodeAs<GeoDataPlacemark>()->setArea( ar );
    }

    return nullptr;
}

}

}
