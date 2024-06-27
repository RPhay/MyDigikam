/*
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace dgml
{

class DgmlStorageLayoutTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser&) const override;
};

} // namespace dgml

} // namespace Marble
