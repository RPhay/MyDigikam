/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace dgml
{

class DgmlDiscreteTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser&) const override;
};

} // namespace dgml

} // namespace Marble
