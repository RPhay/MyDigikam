// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#pragma once

#include "GeoDataTypes.h"
#include "GeoDataTimeStamp.h"

namespace Marble
{

class GeoDataTimeSpanPrivate
{
public:

    GeoDataTimeStamp m_begin;
    GeoDataTimeStamp m_end;
};

} // namespace Marble
