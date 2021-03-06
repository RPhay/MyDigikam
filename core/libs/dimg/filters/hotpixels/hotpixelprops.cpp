/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : HotPixel properties container.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#include "hotpixelprops.h"

// Qt includes

#include <QRegExp>

namespace Digikam
{

int HotPixelProps::y() const
{
    return rect.y();
}

int HotPixelProps::x() const
{
    return rect.x();
}

int HotPixelProps::width() const
{
    return rect.width();
}

int HotPixelProps::height() const
{
    return rect.height();
}

bool HotPixelProps::operator==(const HotPixelProps& p) const
{
    /**
     * NOTE:we can say they're same hotpixel spot if they
     * touch (next to) each other horizontally or vertically, not diagonal corners
     */
    return (
            (rect != p.rect)                            &&
            (
             ((x() + width())  >= p.x())                &&
             ( x()             <= (p.x() + p.width()))  &&
             ((y() + height()) >= p.y())                &&
             ( y()             <= (p.y() + p.height()))
            )                                           &&
            !diagonal(rect, p.rect)
           );
}

bool HotPixelProps::diagonal(const QRect& r1, const QRect& r2) const
{
    // locate next-to positions

    bool top    = ((r1.y() + height() - 1) == (r2.y() - 1)); ///< r1 is on the top of r2
    bool left   = ((r1.x() + width()  - 1) == (r2.x() - 1)); ///< r1 is on the left of r2
    bool right  = (r1.x() == (r2.x() + r2.width()));
    bool bottom = (r1.y() == (r2.y() + r2.height()));

    return (
            (top    && left)    ||
            (top    && right)   ||
            (bottom && left)    ||
            (bottom && right)
           );
}

QString HotPixelProps::toString() const
{
    return (QString::fromUtf8("%1-%2x%3-%4x%5").arg(luminosity)
                                               .arg(rect.x()).arg(rect.y())
                                               .arg(rect.width())
                                               .arg(rect.height()));
}

bool HotPixelProps::fromString(const QString& str)
{
    QRegExp exp(QLatin1String("(\\d+)-(\\d+)x(\\d+)-(\\d+)x(\\d+)"));

    if (exp.exactMatch(str))
    {
        luminosity = exp.cap(1).toInt();
        rect       = QRect(exp.cap(2).toInt(), exp.cap(3).toInt(),
                           exp.cap(4).toInt(), exp.cap(5).toInt());

        return true;
    }

    return false;
}

QStringList HotPixelProps::toStringList(const QList<HotPixelProps>& lst)
{
    QStringList hplst;

    foreach (const HotPixelProps& hp, lst)
    {
        hplst << hp.toString();
    }

    return hplst;
}

QList<HotPixelProps> HotPixelProps::fromStringList(const QStringList& hplst)
{
    QList<HotPixelProps> lst;

    foreach (const QString& str, hplst)
    {
        HotPixelProps hp;

        if (hp.fromString(str))
        {
            lst << hp;
        }
    }

    return lst;
}

} // namespace Digikam
