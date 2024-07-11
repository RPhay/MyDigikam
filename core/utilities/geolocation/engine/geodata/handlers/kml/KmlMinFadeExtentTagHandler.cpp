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

#include "KmlMinFadeExtentTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataLod.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(minFadeExtent)

GeoNode* KmlminFadeExtentTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_minFadeExtent)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Lod))
    {
        float minFadeExtent = parser.readElementText().trimmed().toFloat();

        parentItem.nodeAs<GeoDataLod>()->setMinFadeExtent(minFadeExtent);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
