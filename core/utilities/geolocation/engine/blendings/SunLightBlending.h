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

#include <QtGlobal>

#include "Blending.h"

namespace Marble
{

class SunLocator;

class SunLightBlending: public Blending
{
public:

    explicit SunLightBlending( const SunLocator * sunLocator );
    ~SunLightBlending() override;
    void blend( QImage * const bottom, TextureTile const * const top ) const override;

    void setLevelZeroLayout( int levelZeroColumns, int levelZeroRows );

private:

    static int maxDivisor( int maximum, int fullLength );

    const SunLocator * const m_sunLocator = nullptr;
    int                      m_levelZeroColumns;
    int                      m_levelZeroRows;
};

} // Namespace Marble
