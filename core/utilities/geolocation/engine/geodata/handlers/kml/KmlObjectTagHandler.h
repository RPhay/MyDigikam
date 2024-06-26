// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include <GeoParser.h>
#include <GeoDataObject.h>

namespace Marble
{

class KmlObjectTagHandler
{
public:

    /**
     * Parses the id and targetId attributes and assign their values to the given object
     */
    static void parseIdentifiers( const GeoParser &parser, GeoDataObject* object );

private:
    KmlObjectTagHandler() = delete; // not implemented
};

}
