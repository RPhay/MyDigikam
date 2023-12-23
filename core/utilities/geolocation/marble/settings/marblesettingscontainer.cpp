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
    measurementSys           = group.readEntry("Measurement System",          MarbleLocale::MetricSystem);
    angleUnit                = group.readEntry("Angle Unit",                  Marbleglobal::DecimalDegree);
    stillQ                   = group.readEntry("Still Quality",               MarbleGlobal::HighQuality);
    animationQ               = group.readEntry("Animation Quality",           MarbleGlobal::LowQuality);
    mapFont                  = group.readEntry("Map Font",                    QFont());
    inertialRotation         = group.readEntry("Inertial Rotation",           true);
    mouseRotation            = group.readEntry("Mouse Rotation",              true);
    volatileTileCacheLimit   = group.readEntry("Volatile Tile Cache Limit",   100);
    persistentTileCacheLimit = group.readEntry("Persistent Tile Cache Limit", 999999);
}

void MarbleSettingsContainer::writeToConfig(KConfigGroup& group) const
{
    group.writeEntry("Measurement System",          measurementSys);
    group.writeEntry("Angle Unit",                  angleUnit);
    group.writeEntry("Still Quality",               stillQ);
    group.writeEntry("Animation Quality",           animationQ);
    group.writeEntry("Map Font",                    mapFont);
    group.writeEntry("Inertial Rotation",           (bool)inertialRotation);
    group.writeEntry("Mouse Rotation",              (bool)mouseRotation);
    group.writeEntry("Volatile Tile Cache Limit",   volatileTileCacheLimit);
    group.writeEntry("Persistent Tile Cache Limit", persistentTileCacheLimit);
}

QDebug operator<<(QDebug dbg, const MarbleSettingsContainer& inf)
{
    dbg.nospace() << "[MarbleSettingsContainer] measurementSys("
                  << inf.measurementSys << "), ";
    dbg.nospace() << "angleUnit("
                  << inf.angleUnit << "), ";
    dbg.nospace() << "stillQ("
                  << inf.stillQ << "), ";
    dbg.nospace() << "animationQ("
                  << inf.animationQ << "), ";
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
