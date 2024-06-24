// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#pragma once

#include "GeoDocument.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataAlias;
class GeoDataResourceMapPrivate;

/**
 */
class DIGIKAM_EXPORT GeoDataResourceMap: public GeoNode
{
public:
    GeoDataResourceMap();

    GeoDataResourceMap( const GeoDataResourceMap &other );

    GeoDataResourceMap& operator=( const GeoDataResourceMap &other );

    bool operator==( const GeoDataResourceMap &other ) const;
    bool operator!=( const GeoDataResourceMap &other ) const;

    ~GeoDataResourceMap() override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    const GeoDataAlias& alias() const;

    GeoDataAlias& alias();

    void setAlias( const GeoDataAlias &alias);

    QString sourceHref() const;

    void setSourceHref( const QString& sourceHref );

    QString targetHref() const;

    void setTargetHref( const QString& targetHref );

private:
    GeoDataResourceMapPrivate* const d = nullptr;
};

}
