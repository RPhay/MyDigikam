/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "KmlKeyTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataStyleMap.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(key)

GeoNode* KmlkeyTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_key)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Pair))
    {
        QString content = parser.readElementText().trimmed();

        parentItem.nodeAs<GeoDataStyleMap>()->setLastKey(content);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
