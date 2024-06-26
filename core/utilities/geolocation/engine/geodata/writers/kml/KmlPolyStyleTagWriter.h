// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Thibaut Gridel <tgridel@free.fr>

#pragma once

#include "KmlColorStyleTagWriter.h"

namespace Marble
{

class KmlPolyStyleTagWriter : public KmlColorStyleTagWriter
{
public:
    KmlPolyStyleTagWriter();

protected:
    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;
    bool isEmpty( const GeoNode *node ) const override;
};

}
