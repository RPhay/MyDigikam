// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#pragma once

#include "GeoDataContainer.h"
#include "digikam_export.h"

namespace Marble
{

class GeoDataCreatePrivate;

/**
 */
class DIGIKAM_EXPORT GeoDataCreate : public GeoDataContainer
{
public:
    GeoDataCreate();

    GeoDataCreate( const GeoDataCreate &other );

    ~GeoDataCreate() override;

    GeoDataCreate& operator=( const GeoDataCreate &other );

    bool operator==( const GeoDataCreate &other ) const;
    bool operator!=( const GeoDataCreate &other ) const;

    GeoDataFeature * clone() const override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

private:
    Q_DECLARE_PRIVATE(GeoDataCreate)
};

} // namespace Marble
