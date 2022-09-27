/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : border settings container.
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2010 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
