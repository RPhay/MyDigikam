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

#include "KmlViewBoundScaleTagHandler.h"

#include "GeoDataLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( viewBoundScale )

GeoNode *KmlviewBoundScaleTagHandler::parse(GeoParser & parser) const
{
    Q_ASSERT ( parser.isStartElement() && parser.isValidElement( QString::fromUtf8(kmlTag_viewBoundScale) ) );
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.is<GeoDataLink>() ){
        qreal const viewBoundScale = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLink>()->setViewBoundScale( viewBoundScale );
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
