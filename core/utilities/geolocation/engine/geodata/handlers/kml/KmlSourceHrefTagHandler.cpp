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

#include "KmlSourceHrefTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataAlias.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( sourceHref )

GeoNode* KmlsourceHrefTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_sourceHref)));

    GeoStackItem parentItem = parser.parentElement();

    QString content = parser.readElementText().trimmed();

    if ( parentItem.is<GeoDataAlias>() ){
        parentItem.nodeAs<GeoDataAlias>()->setSourceHref( content );
    }

    return nullptr;
}

}
}
