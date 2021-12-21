/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Managing of focus point items on a GraphicsDImgView
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "focuspointitem.h"

// Qt includes

#include <QPainter>
#include <QPen>

// Local includes

#include "digikam_debug.h"
#include "assignnamewidgetstates.h"

namespace Digikam
{

FocusPointItem::FocusPointItem(QGraphicsItem* const parent)
    : RegionFrameItem(parent),
      m_color        (QColor::fromRgb(0, 0, 0, 255)), ///< alpha is 100 to let more transparency
      m_width        (3)
{
    m_widgetName = new QLabel(nullptr);
/*
    setHudWidget(m_widgetName);
*/
}

FocusPointItem::~FocusPointItem()
{
}

void FocusPointItem::setPoint(const FocusPoint& point)
{
    m_point = point;
    setEditable(false);

    switch (m_point.getType())
    {
        case FocusPoint::TypePoint::Inactive:
        {
            m_color.setAlpha(130);
            m_width = 1;
            break;
        }

        case FocusPoint::TypePoint::Selected:
        case FocusPoint::TypePoint::SelectedInFocus:
        {
            m_color.setRed(255);
            break;
        }

        default: // FocusPoint::TypePoint::InFocus
        {
            // TODO
            break;
        }
    }
}

FocusPoint FocusPointItem::point() const
{
    return m_point;
}

void FocusPointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPen pen;
    pen.setWidth(m_width);
    pen.setColor(m_color);

    QRectF drawRect = boundingRect();
    qCDebug(DIGIKAM_GENERAL_LOG) << "FocusPointsItem: rectangle:" << drawRect;

    painter->setPen(pen);
    painter->drawRect(drawRect);
}

void FocusPointItem::setEditable(bool allowEdit)
{
    changeFlags(GeometryEditable, allowEdit);
}

} // namespace Digikam
