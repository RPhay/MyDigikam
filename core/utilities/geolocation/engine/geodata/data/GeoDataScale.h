// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#pragma once

#include "GeoDataObject.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataScalePrivate;

/**
 */
class DIGIKAM_EXPORT GeoDataScale: public GeoDataObject
{
public:
    GeoDataScale();

    GeoDataScale( const GeoDataScale &other );

    GeoDataScale& operator=( const GeoDataScale &other );

    bool operator==( const GeoDataScale &other ) const;
    bool operator!=( const GeoDataScale &other ) const;

    ~GeoDataScale() override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    double x() const;

    void setX( double x );

    double y() const;

    void setY( double y );

    double z() const;

    void setZ( double z);

private:

    GeoDataScalePrivate* const d = nullptr;
};

} // namespace Marble
