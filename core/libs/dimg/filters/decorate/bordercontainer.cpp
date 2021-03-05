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

#include "bordercontainer.h"

// Qt includes

#include <QStandardPaths>

namespace Digikam
{

BorderContainer::BorderContainer()
    : preserveAspectRatio   (true),
      orgWidth              (0),
      orgHeight             (0),
      borderType            (0),
      borderWidth1          (0),
      borderWidth2          (0),
      borderWidth3          (0),
      borderWidth4          (0),
      borderPercent         (0.1),
      solidColor            (QColor(0, 0, 0)),
      niepceBorderColor     (QColor(255, 255, 255)),
      niepceLineColor       (QColor(0, 0, 0)),
      bevelUpperLeftColor   (QColor(192, 192, 192)),
      bevelLowerRightColor  (QColor(128, 128, 128)),
      decorativeFirstColor  (QColor(0, 0, 0)),
      decorativeSecondColor (QColor(0, 0, 0))
{
}

BorderContainer::~BorderContainer()
{
}

QString BorderContainer::getBorderPath(int border)
{
    QString pattern;

    switch (border)
    {
        case BorderContainer::PineBorder:
            pattern = QLatin1String("pine-pattern");
            break;

        case BorderContainer::WoodBorder:
            pattern = QLatin1String("wood-pattern");
            break;

        case BorderContainer::PaperBorder:
            pattern = QLatin1String("paper-pattern");
            break;

        case BorderContainer::ParqueBorder:
            pattern = QLatin1String("parque-pattern");
            break;

        case BorderContainer::IceBorder:
            pattern = QLatin1String("ice-pattern");
            break;

        case BorderContainer::LeafBorder:
            pattern = QLatin1String("leaf-pattern");
            break;

        case BorderContainer::MarbleBorder:
            pattern = QLatin1String("marble-pattern");
            break;

        case BorderContainer::RainBorder:
            pattern = QLatin1String("rain-pattern");
            break;

        case BorderContainer::CratersBorder:
            pattern = QLatin1String("craters-pattern");
            break;

        case BorderContainer::DriedBorder:
            pattern = QLatin1String("dried-pattern");
            break;

        case BorderContainer::PinkBorder:
            pattern = QLatin1String("pink-pattern");
            break;

        case BorderContainer::StoneBorder:
            pattern = QLatin1String("stone-pattern");
            break;

        case BorderContainer::ChalkBorder:
            pattern = QLatin1String("chalk-pattern");
            break;

        case BorderContainer::GraniteBorder:
            pattern = QLatin1String("granit-pattern");
            break;

        case BorderContainer::RockBorder:
            pattern = QLatin1String("rock-pattern");
            break;

        case BorderContainer::WallBorder:
            pattern = QLatin1String("wall-pattern");
            break;

        default:
            return pattern;
            break;
    }

    return (QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/") + pattern + QLatin1String(".png")));
}

} // namespace Digikam
