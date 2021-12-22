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

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN FocusPoint::Private : public QSharedData
{
public:

    explicit Private()
      : x_position(0.0),
        y_position(0.0),
        width     (0.0),
        height    (0.0),
        type      (Inactive)
    {
    };

    float     x_position;       ///< X coordinate of the center of focus point area.
    float     y_position;       ///< Y coordinate of the center of focus point area.
    float     width;            ///< Width of focus point area.
    float     height;           ///< Height of focus point area.
    TypePoint type;             ///< Focus point area type. See TypePoint enum definition for details.
};

FocusPoint::FocusPoint()
    : d(new Private)
{
}

FocusPoint::FocusPoint(float x_position, float y_position, float width, float height, TypePoint type)
    : d(new Private)
{
    d->x_position = x_position;
    d->y_position = y_position;
    d->width      = width;
    d->height     = height;
    d->type       = type;
}

FocusPoint::FocusPoint(float x_position, float y_position, float width, float height)
    : d(new Private)
{
    d->x_position = x_position;
    d->y_position = y_position;
    d->width      = width;
    d->height     = height;
    d->type       = Inactive;
}

FocusPoint::FocusPoint(const QRectF& rectF)
    : d(new Private)
{
    d->x_position = rectF.topLeft().x() + rectF.width()  * 0.5;
    d->y_position = rectF.topLeft().y() + rectF.height() * 0.5;
    d->width      = rectF.width();
    d->height     = rectF.height();
    d->type       = Inactive;
}

FocusPoint::FocusPoint(const FocusPoint& other)
    : d(other.d)
{
}

FocusPoint::~FocusPoint()
{
}

FocusPoint& FocusPoint::operator=(const FocusPoint& other)
{
    d = other.d;

    return *this;
}

void FocusPoint::setType(TypePoint type)
{
    d->type = type;
}

FocusPoint::TypePoint FocusPoint::getType() const
{
    return d->type;
}

QString FocusPoint::getTypeDescription() const
{
    switch (getType())
    {
        case InFocus:
        {
            return i18nc("Focus point type description", "In Focus");
        }

        case Selected:
        {
            return i18nc("Focus point type description", "Selected");
        }

        case SelectedInFocus:
        {
            return i18nc("Focus point type description", "Selected In Focus");
        }

        default: // Inactive
        {
            return i18nc("Focus point type description", "Inactive");
        }
    }
}

QRect FocusPoint::getRectBySize(const QSize& size) const
{
    return QRect(static_cast<int>((d->x_position - 0.5 * d->width)  * size.width()),
                 static_cast<int>((d->y_position - 0.5 * d->height) * size.height()),
                 static_cast<int>(d->width  * size.width()),
                 static_cast<int>(d->height * size.height()));
}

void FocusPoint::setCenterPosition(float x_position, float y_position)
{
    d->x_position = x_position;
    d->y_position = y_position;
}

void FocusPoint::setSize(float width, float height)
{
    d->width  = width;
    d->height = height;
}

QPointF FocusPoint::getCenterPosition() const
{
    return QPointF(d->x_position, d->y_position);
}

QSizeF FocusPoint::getSize() const
{
    return QSizeF(d->width, d->height);
}

QRectF FocusPoint::getRect() const
{
    QRectF rect;
    rect.setSize(getSize());
    rect.moveCenter(getCenterPosition());

    return rect;
}

QDebug operator<<(QDebug dbg, const FocusPoint& fp)
{
    dbg.nospace() << "FocusPoint::area:"
                  << fp.getRect() << ", ";
    dbg.nospace() << "FocusPoint::type:"
                  << fp.getTypeDescription();

    return dbg.space();
}

} // namespace Digikam
