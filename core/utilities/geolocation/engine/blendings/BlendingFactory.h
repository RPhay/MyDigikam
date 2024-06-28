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

#include <QHash>

class QString;

namespace Marble
{

class Blending;
class SunLightBlending;
class SunLocator;

class BlendingFactory
{
public:

    explicit BlendingFactory( const SunLocator *sunLocator );
    ~BlendingFactory();

    void setLevelZeroLayout( int levelZeroColumns, int levelZeroRows );

    Blending const * findBlending( QString const & name ) const;

private:

    Q_DISABLE_COPY(BlendingFactory)

    SunLightBlending* const m_sunLightBlending = nullptr;
    QHash<QString, Blending const *> m_blendings;
};

} // Namespace Marble
