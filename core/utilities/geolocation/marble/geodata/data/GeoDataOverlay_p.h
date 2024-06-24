// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "GeoDataFeature_p.h"

#include <QImage>
#include <QColor>

namespace Marble {

class GeoDataOverlayPrivate : public GeoDataFeaturePrivate
{
public:
    QColor m_color;

    int m_drawOrder;

    mutable QImage m_image;

    QString m_iconPath;

    GeoDataOverlayPrivate()
        : m_color(Qt::white),
          m_drawOrder(0)
    {
    }

    GeoDataOverlayPrivate(const GeoDataOverlayPrivate& other)
      : GeoDataFeaturePrivate(other),
        m_color(Qt::white),
        m_drawOrder(0)
    {
    }
};

}
