/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Extraction of focus points by exiftool data
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

#include "focuspoint.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

FocusPoint::FocusPoint()
    : x_position(0.0),
      y_position(0.0),
      width     (0.0),
      height    (0.0),
      type      (Inactive)
{
}

FocusPoint::FocusPoint(float x_position, float y_position, float width, float height, TypePoint type)
{
    this->x_position = x_position;
    this->y_position = y_position;
    this->width      = width;
    this->height     = height;
    this->type       = type;
}

FocusPoint::FocusPoint(float x_position, float y_position, float width, float height)
{
    this->x_position = x_position;
    this->y_position = y_position;
    this->width      = width;
    this->height     = height;
    this->type       = Inactive;
}

FocusPoint::FocusPoint(const QRectF& rectF)
{
    this->x_position = rectF.topLeft().x() + rectF.width()  * 0.5;
    this->y_position = rectF.topLeft().y() + rectF.height() * 0.5;
    this->width      = rectF.width();
    this->height     = rectF.height();
    this->type       = Inactive;
}

FocusPoint::~FocusPoint()
{
}

void FocusPoint::setType(TypePoint type)
{
    this->type       = type;
}

FocusPoint::TypePoint FocusPoint::getType() const
{
    return type;
}

QRect FocusPoint::getRectBySize(const QSize& size) const
{
    return QRect(static_cast<int>((x_position - 0.5 * width)  * size.width()),
                 static_cast<int>((y_position - 0.5 * height) * size.height()),
                 static_cast<int>(width  * size.width()),
                 static_cast<int>(height * size.height()));
}

void FocusPoint::setPosition(float x_position, float y_position)
{
    this->x_position = x_position;
    this->y_position = y_position;
}

void FocusPoint::setSize(float width, float height)
{
    this->width  = width;
    this->height = height;
}

QPointF FocusPoint::getPosition() const
{
    return QPointF(x_position, y_position);
}

QSizeF FocusPoint::getSize() const
{
    return QSizeF(width, height);
}

QRectF FocusPoint::getRect() const
{
    return QRectF(x_position, y_position, width, height);
}

} // namespace Digikam
