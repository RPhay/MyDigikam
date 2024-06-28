// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "KmlColorStyleTagWriter.h"

namespace Marble
{

class KmlLineStyleTagWriter : public KmlColorStyleTagWriter
{
public:

    KmlLineStyleTagWriter();

protected:

    bool writeMid( const GeoNode *node, GeoWriter& writer ) const override;

    bool isEmpty( const GeoNode *node ) const override;
};

} // namespace Marble
