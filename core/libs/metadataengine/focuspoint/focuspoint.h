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

#ifndef DIGIKAM_FOCUSPOINT_H
#define DIGIKAM_FOCUSPOINT_H

// Qt includes

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QRectF>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FocusPoint
{
public:

    enum TypePoint
    {
        Inactive        = 0,
        Infocus         = 1,
        Selected        = 2,
        SelectedInFocus = 3
    };

public:

    FocusPoint();
    FocusPoint(float x_position, float y_position, float width, float height, TypePoint type);
    FocusPoint(float x_position, float y_position, float width, float height);
    explicit FocusPoint(const QRectF& rectF);
    ~FocusPoint();

    void setType(TypePoint type);
    TypePoint getType()                                     const;
    QRect getRectBySize(const QSize& size)                  const;

    void setPosition(float x_position, float y_position);
    void setSize(float width, float height);
    QPointF getPosition()                                   const;
    QSizeF  getSize()                                       const;

private:

    float     x_position;
    float     y_position;
    float     width;
    float     height;
    TypePoint type;
};

inline FocusPoint::TypePoint operator|(FocusPoint::TypePoint type1, FocusPoint::TypePoint type2)
{
    return static_cast<FocusPoint::TypePoint>(static_cast<int>(type1) | static_cast<int>(type2));
}

inline FocusPoint::TypePoint operator&(FocusPoint::TypePoint type1, FocusPoint::TypePoint type2)
{
    return static_cast<FocusPoint::TypePoint>(static_cast<int>(type1) & static_cast<int>(type2));
}

inline FocusPoint::TypePoint& operator|=(FocusPoint::TypePoint& type1, FocusPoint::TypePoint type2)
{
    return (type1 = type1 | type2);
}

inline FocusPoint::TypePoint& operator&=(FocusPoint::TypePoint& type1, FocusPoint::TypePoint type2)
{
    return (type1 = type1 & type2);
}

} // namespace Digikam

#endif // DIGIKAM_FOCUSPOINT_H
