// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#pragma once

#include "GeoDataTourPrimitive.h"

namespace Marble
{

/**
 */
class DIGIKAM_EXPORT GeoDataWait : public GeoDataTourPrimitive
{
public:

    GeoDataWait();
    ~GeoDataWait() override;

    bool operator==(const GeoDataWait &other) const;
    bool operator!=(const GeoDataWait &other) const;
    const char *nodeType() const override;

    double duration() const;
    void setDuration(double duration);

private:
    double m_duration;
};

} // namespace Marble
