// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#pragma once

namespace Marble
{

class GeoDataLineString;
class GeoWriter;
class OsmPlacemarkData;

class OsmWayTagWriter
{
public:

    static void writeWay( const GeoDataLineString& lineString,
                          const OsmPlacemarkData& osmData, GeoWriter& writer );
};

} // namespace Marble
