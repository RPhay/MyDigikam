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

#include "DgmlIconTagHandler.h"

// Qt includes

#include <QColor>

// Local includes

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneItem.h"

namespace Marble
{

namespace dgml
{

DGML_DEFINE_TAG_HANDLER(Icon)

GeoNode* DgmlIconTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Icon)));

    QString pixmapRelativePath  = parser.attribute(dgmlAttr_pixmap).trimmed();

    QColor color;
    color.setNamedColor(parser.attribute(dgmlAttr_color).trimmed());

    GeoSceneIcon* icon = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(dgmlTag_Head))
    {
        icon = parentItem.nodeAs<GeoSceneHead>()->icon();
        icon->setPixmap(pixmapRelativePath);
        icon->setColor(color);
    }

    if (parentItem.represents(dgmlTag_Item))
    {
        icon = parentItem.nodeAs<GeoSceneItem>()->icon();
        icon->setPixmap(pixmapRelativePath);
        icon->setColor(color);
    }

    return nullptr;
}

} // namespace dgml

} // namespace Marble
