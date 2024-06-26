/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "GeoTagHandler.h"

#include "GeoDataSimpleField.h"

class QString;

namespace Marble
{

namespace kml
{

class KmlSimpleFieldTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;

private:
    static GeoDataSimpleField::SimpleFieldType resolveType( const QString& type );
};

}

}
