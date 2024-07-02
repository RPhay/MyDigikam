/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

#include "GeoDataContainer.h"

#include "digikam_export.h"

namespace Marble
{

class GeoDataDeletePrivate;

class DIGIKAM_EXPORT GeoDataDelete : public GeoDataContainer
{
public:

    GeoDataDelete();

    GeoDataDelete( const GeoDataDelete &other );

    ~GeoDataDelete() override;

    GeoDataDelete& operator=( const GeoDataDelete &other );

    bool operator==( const GeoDataDelete &other ) const;
    bool operator!=( const GeoDataDelete &other ) const;

    GeoDataFeature * clone() const override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

private:

    Q_DECLARE_PRIVATE(GeoDataDelete)
};

} // namespace Marble
