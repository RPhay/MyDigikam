// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "GeoDataPoint.h"
#include "GeoGraphicsItem.h"
#include "digikam_export.h"

#include <QImage>

namespace Marble
{

class DIGIKAM_EXPORT GeoPhotoGraphicsItem : public GeoGraphicsItem
{
public:
    explicit GeoPhotoGraphicsItem( const GeoDataFeature *feature );

    void setPoint( const GeoDataPoint& point );

    GeoDataPoint point() const;

    void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel) override;

    const GeoDataLatLonAltBox& latLonAltBox() const override;

    bool contains(const QPoint &point, const ViewportParams *viewport) const override;

protected:
    GeoDataPoint m_point;

    QImage m_photo;
};

} // namespace Marble
