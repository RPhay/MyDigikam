// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Sanjiban Bairagya <sanjibanb@kde.org>
//

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace dgml
{

class DgmlRenderOrderTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser&) const override;
};

} // namespace dgml

} // namespace Marble
