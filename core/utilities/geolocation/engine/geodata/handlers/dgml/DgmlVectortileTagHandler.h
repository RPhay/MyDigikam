/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace dgml
{

class DgmlVectortileTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse( GeoParser& ) const override;
};

} // namespace dgml

} // namespace Marble
