/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : border settings container.
 *
 * Copyright 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright 2009-2010 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_BORDER_CONTAINER_H
#define DIGIKAM_BORDER_CONTAINER_H

// Qt includes

#include <QColor>
#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT BorderContainer
{

public:

    enum BorderTypes
    {
        SolidBorder = 0,
        NiepceBorder,
        BeveledBorder,
        PineBorder,
        WoodBorder,
        PaperBorder,
        ParqueBorder,
        IceBorder,
        LeafBorder,
        MarbleBorder,
        RainBorder,
        CratersBorder,
        DriedBorder,
        PinkBorder,
        StoneBorder,
        ChalkBorder,
        GraniteBorder,
        RockBorder,
        WallBorder
    };

public:

    explicit BorderContainer();
    ~BorderContainer();

    static QString getBorderPath(int border);

public:

    bool    preserveAspectRatio;

    int     orgWidth;
    int     orgHeight;

    int     borderType;

    int     borderWidth1;
    int     borderWidth2;
    int     borderWidth3;
    int     borderWidth4;

    double  borderPercent;

    QString borderPath;

    QColor  solidColor;
    QColor  niepceBorderColor;
    QColor  niepceLineColor;
    QColor  bevelUpperLeftColor;
    QColor  bevelLowerRightColor;
    QColor  decorativeFirstColor;
    QColor  decorativeSecondColor;
};

} // namespace Digikam

#endif // DIGIKAM_BORDER_CONTAINER_H
