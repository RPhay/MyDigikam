/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlRoleTagHandler.h"

#include "digikam_debug.h"

#include "KmlElementDictionary.h"

#include "GeoDataPlacemark.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( role )

GeoNode* KmlroleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_role)));

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.is<GeoDataPlacemark>() ) {
        QString role = parser.readElementText().trimmed();
        if ( role.isEmpty() ) {
            role = QLatin1Char(' ');
        }
        parentItem.nodeAs<GeoDataPlacemark>()->setRole( role );
    }

    return nullptr;
}

}

}
