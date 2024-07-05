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

#include <QSet>

namespace Marble
{

class OsmPlacemarkData;
class GeoWriter;

class OsmTagTagWriter
{

public:

    /**
     * @brief writeTags writes multiple OSM \<tag k="key" v="value"\>
     */
    static void writeTags( const OsmPlacemarkData &osmData, GeoWriter& writer );

private:

    static QSet<QString> m_blacklist;
};

} // namespace Marble
