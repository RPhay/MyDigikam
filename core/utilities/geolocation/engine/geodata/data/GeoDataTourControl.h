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

namespace Marble
{

class DIGIKAM_EXPORT GeoDataTourControl : public GeoDataTourPrimitive
{
public:

    enum PlayMode
    {
        Play,
        Pause
    };

    GeoDataTourControl();
    ~GeoDataTourControl() override;

    bool operator==( const GeoDataTourControl &other ) const;
    bool operator!=( const GeoDataTourControl &other ) const;
    const char *nodeType() const override;

    PlayMode playMode() const;
    void setPlayMode(PlayMode mode);

private:

    PlayMode m_playMode;
};

} // namespace Marble
