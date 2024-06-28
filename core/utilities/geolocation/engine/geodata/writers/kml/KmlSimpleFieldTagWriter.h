// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#pragma once

#include "GeoTagWriter.h"
#include "GeoDataSimpleField.h"

class QString;

namespace Marble
{

class KmlSimpleFieldTagWriter : public GeoTagWriter
{
public:

    bool write( const GeoNode *node, GeoWriter& writer ) const override;

private:

    static QString resolveType( GeoDataSimpleField::SimpleFieldType type );
};

} // namespace Marble
