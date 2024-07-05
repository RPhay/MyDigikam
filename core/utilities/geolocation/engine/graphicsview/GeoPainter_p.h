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

#include "MarbleGlobal.h"

class QPolygonF;
class QSizeF;
class QPainterPath;
class QRectF;

namespace Marble
{

class ViewportParams;
class GeoDataCoordinates;
class GeoPainter;

struct Q_DECL_HIDDEN GeoPainterPrivate
{
public:

    GeoPainterPrivate( GeoPainter* q, const ViewportParams *viewport, MapQuality mapQuality );

    ~GeoPainterPrivate();


    static void createAnnotationLayout ( qreal x, qreal y,
                                         const QSizeF& bubbleSize,
                                         qreal bubbleOffsetX, qreal bubbleOffsetY,
                                         qreal xRnd, qreal yRnd,
                                         QPainterPath& path, QRectF& rect );

    static GeoDataLinearRing createLinearRingFromGeoRect( const GeoDataCoordinates & centerCoordinates,
                                                          qreal width, qreal height );

    static bool doClip( const ViewportParams *viewport );

    static qreal normalizeAngle(qreal angle);

    void drawTextRotated( const QPointF &startPoint, qreal angle, const QString &text );

public:

    const ViewportParams* const m_viewport      = nullptr;
    const MapQuality            m_mapQuality;
    qreal* const                m_x             = nullptr;

private:

    GeoPainter* m_parent                        = nullptr;
};

} // namespace Marble
