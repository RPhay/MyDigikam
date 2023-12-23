/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings Container.
 *
 * SPDX-FileCopyrightText: 2022-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "marblesettingscontainer.h"

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "marblesettings.h"

namespace Digikam
{

void MarbleSettingsContainer::readFromConfig(KConfigGroup& group)
{
    distanceUnit             = (MarbleLocale::MeasurementSystem)group.readEntry("Distance Unit",               (int)MarbleLocale::MetricSystem);
    angleUnit                = (Marble::AngleUnit)              group.readEntry("Angle Unit",                  (int)Marble::DecimalDegree);
    stillQuality             = (Marble::MapQuality)             group.readEntry("Still Quality",               (int)Marble::HighQuality);
    animationQuality         = (Marble::MapQuality)             group.readEntry("Animation Quality",           (int)Marble::LowQuality);
    mapFont                  = group.readEntry("Map Font",                    QFont());
    inertialRotation         = group.readEntry("Inertial Rotation",           true);
    mouseRotation            = group.readEntry("Mouse Rotation",              true);
    volatileTileCacheLimit   = group.readEntry("Volatile Tile Cache Limit",   100);
    persistentTileCacheLimit = group.readEntry("Persistent Tile Cache Limit", 999999);
}

void MarbleSettingsContainer::writeToConfig(KConfigGroup& group) const
{
    group.writeEntry("Distance Unit",               (int)distanceUnit);
    group.writeEntry("Angle Unit",                  (int)angleUnit);
    group.writeEntry("Still Quality",               (int)stillQuality);
    group.writeEntry("Animation Quality",           (int)animationQuality);
    group.writeEntry("Map Font",                    mapFont);
    group.writeEntry("Inertial Rotation",           (bool)inertialRotation);
    group.writeEntry("Mouse Rotation",              (bool)mouseRotation);
    group.writeEntry("Volatile Tile Cache Limit",   volatileTileCacheLimit);
    group.writeEntry("Persistent Tile Cache Limit", persistentTileCacheLimit);
}

QDebug operator<<(QDebug dbg, const MarbleSettingsContainer& inf)
{
    dbg.nospace() << "[MarbleSettingsContainer] distanceUnit("
                  << inf.distanceUnit << "), ";
    dbg.nospace() << "angleUnit("
                  << inf.angleUnit << "), ";
    dbg.nospace() << "stillQuality("
                  << inf.stillQuality << "), ";
    dbg.nospace() << "animationQuality("
                  << inf.animationQuality << "), ";
    dbg.nospace() << "mapFont("
                  << inf.mapFont << "), ";
    dbg.nospace() << "inertialRotation("
                  << inf.inertialRotation << "), ";
    dbg.nospace() << "mouseRotation("
                  << inf.mouseRotation << "), ";
    dbg.nospace() << "volatileTileCacheLimit("
                  << inf.volatileTileCacheLimit << "), ";
    dbg.nospace() << "persistentTileCacheLimit("
                  << inf.persistentTileCacheLimit << ")";

    return dbg.space();
}

} // namespace Digikam
