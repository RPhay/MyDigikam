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

#include "GeoDataTourPrimitive.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataAnimatedUpdatePrivate;
class GeoDataUpdate;

/**
 */
class DIGIKAM_EXPORT GeoDataAnimatedUpdate: public GeoDataTourPrimitive
{
public:

    GeoDataAnimatedUpdate();
    GeoDataAnimatedUpdate( const GeoDataAnimatedUpdate &other );
    GeoDataAnimatedUpdate& operator=( const GeoDataAnimatedUpdate &other );
    bool operator== (const GeoDataAnimatedUpdate &other) const;
    bool operator!= (const GeoDataAnimatedUpdate &other) const;
    ~GeoDataAnimatedUpdate() override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    double duration() const;
    void setDuration( double duration );

    double delayedStart() const;
    void setDelayedStart( double delayedStart );

    const GeoDataUpdate* update() const;
    GeoDataUpdate* update();
    void setUpdate( GeoDataUpdate *update );

private:

    GeoDataAnimatedUpdatePrivate* const d = nullptr;
};

} // namespace Marble
