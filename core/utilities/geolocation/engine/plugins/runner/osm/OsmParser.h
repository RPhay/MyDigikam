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

#pragma once

#include <QString>

#include "OsmNode.h"
#include "OsmWay.h"
#include "OsmRelation.h"

namespace Marble
{

class GeoDataDocument;

class OsmParser
{
public:

    static GeoDataDocument* parse(const QString &filename, QString &error);

private:

    static GeoDataDocument* parseXml(const QString &filename, QString &error);
    static GeoDataDocument* parseO5m(const QString &filename, QString &error);
    static GeoDataDocument* parseOsmPbf(const QString &filename, QString &error);
    static GeoDataDocument *createDocument(OsmNodes &nodes, OsmWays &way, OsmRelations &relations);
};

} // namespace Marble
