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

#include <QVariant>
#include <QStringList>
#include <QRectF>
#include <QDebug>
#include <QExplicitlySharedDataPointer>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FocusPoint
{
public:

    enum TypePoint
    {
        Inactive        = 0,    ///< The AF-point is not active.
        InFocus         = 1,    ///< The AF-point is in focus.
        Selected        = 2,    ///< The AF-point is selected but not in focus.
        SelectedInFocus = 3     ///< The AF-point is selected and in focus.
    };

public:

    FocusPoint();
    FocusPoint(float x_position, float y_position, float width, float height, TypePoint type);
    FocusPoint(float x_position, float y_position, float width, float height);
    explicit FocusPoint(const QRectF& rectF);
    FocusPoint(const FocusPoint& other);
    ~FocusPoint();

    /**
     * Equivalent to the copy constructor
     */
    FocusPoint& operator=(const FocusPoint& other);

    void      setType(TypePoint type);
    TypePoint getType()                                     const;
    QString   getTypeDescription()                          const;

    void setCenterPosition(float x_position, float y_position);
    void setSize(float width, float height);
    QPointF getCenterPosition()                             const;
    QSizeF  getSize()                                       const;
    QRectF  getRect()                                       const;
    QRect   getRectBySize(const QSize& size)                const;

private:

    class Private;
    QExplicitlySharedDataPointer<Private> d;
};

inline FocusPoint::TypePoint operator|(FocusPoint::TypePoint type1, FocusPoint::TypePoint type2)
{
    return (static_cast<FocusPoint::TypePoint>(static_cast<int>(type1) | static_cast<int>(type2)));
}

inline FocusPoint::TypePoint operator&(FocusPoint::TypePoint type1, FocusPoint::TypePoint type2)
{
    return (static_cast<FocusPoint::TypePoint>(static_cast<int>(type1) & static_cast<int>(type2)));
}

inline FocusPoint::TypePoint& operator|=(FocusPoint::TypePoint& type1, FocusPoint::TypePoint type2)
{
    return (type1 = type1 | type2);
}

inline FocusPoint::TypePoint& operator&=(FocusPoint::TypePoint& type1, FocusPoint::TypePoint type2)
{
    return (type1 = type1 & type2);
}

//! qDebug() stream operator. Writes property @fp to the debug output in a nicely formatted way.
DIGIKAM_EXPORT QDebug operator<<(QDebug dbg, const FocusPoint& fp);

} // namespace Digikam

#endif // DIGIKAM_FOCUSPOINT_H
