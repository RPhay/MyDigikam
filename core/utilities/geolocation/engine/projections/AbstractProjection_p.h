/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

namespace Marble
{

class AbstractProjection;

class Q_DECL_HIDDEN AbstractProjectionPrivate
{
public:

    explicit AbstractProjectionPrivate(AbstractProjection* parent);

    virtual ~AbstractProjectionPrivate() {};

    int levelForResolution(qreal resolution) const;

    Q_DECLARE_PUBLIC(AbstractProjection)

public:

    qreal                       m_maxLat;
    qreal                       m_minLat;
    mutable qreal               m_previousResolution;
    mutable qreal               m_level;

    AbstractProjection* const   q_ptr = nullptr;
};

} // namespace Marble
