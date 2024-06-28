/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlrangeTagHandler:public GeoTagHandler
{
public:

    GeoNode * parse (GeoParser &) const override;
};

} // namespace kml

} // namespace Marble
