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

#include <QAtomicInt>

#include "GeoDataCoordinates.h"

namespace Marble
{

class GeoDataCameraPrivate
{
public:

    GeoDataCameraPrivate()
        : m_coordinates(),
          m_roll( 0.0 ),
          m_heading( 0.0 ),
          m_tilt( 0.0 ),
          ref( 1 )
    {
    }

    GeoDataCoordinates  m_coordinates;
    qreal               m_roll;
    qreal               m_heading;
    qreal               m_tilt;

    QAtomicInt          ref;
};

} // namespace Marble
