/*
    SPDX-FileCopyrightText: 2010 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-FileCopyrightText: 2011 Anders Lund <anders@alweb.dk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace gpx
{

class GPXrteTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser&) const override;
};

} // namespace gpx

} // namespace Marble
