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

#include "KmlStateTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataItemIcon.h"
#include "GeoDataParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( state )

GeoNode* KmlstateTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_state)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataItemIcon::ItemIconStates itemIconState;

    if ( parentItem.represents( kmlTag_ItemIcon ) )
    {
        QString value = parser.readElementText().trimmed();
        QStringList iconStateTextList = value.split(QLatin1Char(' '));

        for( const QString &value: iconStateTextList ) {
            if (value == QLatin1String("open")) {
                itemIconState |= GeoDataItemIcon::Open;
            } else if (value == QLatin1String("closed")) {
                itemIconState |= GeoDataItemIcon::Closed;
            } else if (value == QLatin1String("error")) {
                itemIconState |= GeoDataItemIcon::Error;
            } else if (value == QLatin1String("fetching0")) {
                itemIconState |= GeoDataItemIcon::Fetching0;
            } else if (value == QLatin1String("fetching1")) {
                itemIconState |= GeoDataItemIcon::Fetching1;
            } else if (value == QLatin1String("fetching2")) {
                itemIconState |= GeoDataItemIcon::Fetching2;
            }
            else {
                qCDebug(DIGIKAM_MARBLE_LOG) << "Cannot parse state value" << value;
            }
        }

        parentItem.nodeAs<GeoDataItemIcon>()->setState( itemIconState );
    }
    return nullptr;
}

}
}
