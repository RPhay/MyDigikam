// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#pragma once

#include "GeoDataObject.h"
#include "digikam_export.h"

namespace Marble
{

/**
 */
class DIGIKAM_EXPORT GeoDataTourPrimitive : public GeoDataObject
{
public:
    bool operator==(const GeoDataTourPrimitive &other) const;
    inline bool operator!=(const GeoDataTourPrimitive &other) const { return !(*this == other); }
};

} // namespace Marble
