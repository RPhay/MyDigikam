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

#include "MarbleGraphicsItem_p.h"

#include "BillboardGraphicsItem.h"
#include "ScreenGraphicsItem.h"
#include "ViewportParams.h"

#include "digikam_debug.h"

namespace Marble
{

struct Q_DECL_HIDDEN ScreenGraphicsItemPrivate : public MarbleGraphicsItemPrivate
{
public:

    ScreenGraphicsItemPrivate( ScreenGraphicsItem *screenGraphicsItem,
                               MarbleGraphicsItem *parent )
        : MarbleGraphicsItemPrivate( screenGraphicsItem, parent ),
          m_position(),
          m_viewportSize(),
          m_floatItemMoving( false ),
          m_flags( ScreenGraphicsItem::GraphicsItemFlags() )
    {
    }

    ~ScreenGraphicsItemPrivate() override
    {
    }

    QVector<QPointF> positions() const override
    {
        QVector<QPointF> list;

        list.append( positivePosition() );
        return list;
    }

    QPointF positivePosition() const
    {
        const QSizeF parentSize = m_parent ? m_parent->size() : m_viewportSize;
        if ( !parentSize.isValid() ) {
            qCDebug(DIGIKAM_MARBLE_LOG) << "Invalid parent size";
            return m_position;
        }
        QPointF position;
        qreal x = m_position.x();
        qreal y = m_position.y();

        position.setX( ( x >= 0 ) ? x : parentSize.width() + x - m_size.width() );
        position.setY( ( y >= 0 ) ? y : parentSize.height() + y - m_size.height() );

        return position;
    }

    QVector<QPointF> absolutePositions() const override
    {
        if( m_parent == nullptr ) {
            return positions();
        }

        QVector<QPointF> parentPositions;

        if( ScreenGraphicsItem *screenItem = dynamic_cast<ScreenGraphicsItem*>( m_parent ) ) {
            parentPositions = screenItem->absolutePositions();
        }
        else if ( BillboardGraphicsItem *geoLabelItem = dynamic_cast<BillboardGraphicsItem *>( m_parent ) ) {
            parentPositions = geoLabelItem->positions();
        }

        QPointF relativePosition = positivePosition();

        QVector<QPointF> absolutePositions;
        absolutePositions.reserve(parentPositions.size());
        for( const QPointF &point: parentPositions ) {
            absolutePositions.append( point + relativePosition );
        }

        return absolutePositions;
    }

    void setProjection(const ViewportParams *viewport) override
    {
        // If we have no parent
        if( m_parent == nullptr ) {
            // Saving the screen size needed for positions()
            m_viewportSize = viewport->size();
        }
    }

    bool isMovable() const
    {
        return ( m_flags & ScreenGraphicsItem::ItemIsMovable ) ? true : false;
    }

public:

    QPointF                                 m_position;
    // The size of the parent, or if no parent exists the size of the viewport.
    QSizeF                                  m_viewportSize;

    QPoint                                  m_floatItemMoveStartPos;
    bool                                    m_floatItemMoving;

    ScreenGraphicsItem::GraphicsItemFlags   m_flags;
};

} // Namespace Marble
