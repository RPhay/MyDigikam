// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2009 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>


#include "MarbleGlobal.h"
//#include "GeoPainter.h"

class QPolygonF;
class QSizeF;
class QPainterPath;
class QRectF;

namespace Marble
{

class ViewportParams;
class GeoDataCoordinates;
class GeoPainter;

class GeoPainterPrivate
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

    const ViewportParams* const m_viewport      = nullptr;
    const MapQuality            m_mapQuality;
    qreal* const                m_x             = nullptr;

private:

    GeoPainter* m_parent                        = nullptr;
};

} // namespace Marble
