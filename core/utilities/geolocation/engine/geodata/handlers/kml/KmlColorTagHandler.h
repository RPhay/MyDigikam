/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "GeoTagHandler.h"

#include <QColor>

namespace Marble
{
namespace kml
{

class KmlcolorTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;

    static QColor parseColor( const QString &colorString );
};

}

}
