/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings Container.
 *
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "geolocationsettingscontainer.h"

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "geolocationsettings.h"
#include "digikam_globals.h"

namespace Digikam
{

void GeolocationSettingsContainer::readFromConfig(KConfigGroup& group)
{
    distanceUnit             = (MarbleLocale::MeasurementSystem)group.readEntry("Distance Unit",     (int)MarbleLocale::MetricSystem);
    angleUnit                = (Marble::AngleUnit)              group.readEntry("Angle Unit",        (int)Marble::DecimalDegree);
    stillQuality             = (Marble::MapQuality)             group.readEntry("Still Quality",     (int)Marble::HighQuality);
    animationQuality         = (Marble::MapQuality)             group.readEntry("Animation Quality", (int)Marble::LowQuality);
    mapFont                  = group.readEntry("Map Font",                    QFont());
    inertialRotation         = group.readEntry("Inertial Rotation",           true);
    mouseRotation            = group.readEntry("Mouse Rotation",              true);
    volatileTileCacheLimit   = group.readEntry("Volatile Tile Cache Limit",   100);
    persistentTileCacheLimit = group.readEntry("Persistent Tile Cache Limit", 999999);
    showGrid                 = group.readEntry("Show Grid",                   true);
    showCross                = group.readEntry("Show Cross",                  true);
}

void GeolocationSettingsContainer::writeToConfig(KConfigGroup& group) const
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
    group.writeEntry("Show Grid",                   (bool)showGrid);
    group.writeEntry("Show Cross",                  (bool)showCross);
}

QDebug operator<<(QDebug dbg, const GeolocationSettingsContainer& inf)
{
    dbg.nospace() << "[GeolocationSettingsContainer]"       << QT_ENDL;
    dbg.nospace() << "   distanceUnit("
                  << inf.distanceUnit << "), "              << QT_ENDL;
    dbg.nospace() << "   angleUnit("
                  << inf.angleUnit << "), "                 << QT_ENDL;
    dbg.nospace() << "   stillQuality("
                  << inf.stillQuality << "), "              << QT_ENDL;
    dbg.nospace() << "   animationQuality("
                  << inf.animationQuality << "), "          << QT_ENDL;
    dbg.nospace() << "   mapFont("
                  << inf.mapFont << "), "                   << QT_ENDL;
    dbg.nospace() << "   inertialRotation("
                  << inf.inertialRotation << "), "          << QT_ENDL;
    dbg.nospace() << "   mouseRotation("
                  << inf.mouseRotation << "), "             << QT_ENDL;
    dbg.nospace() << "   volatileTileCacheLimit("
                  << inf.volatileTileCacheLimit << "), "    << QT_ENDL;
    dbg.nospace() << "   persistentTileCacheLimit("
                  << inf.persistentTileCacheLimit << "), "  << QT_ENDL;
    dbg.nospace() << "   showGrid("
                  << inf.showGrid << "), "  << QT_ENDL;
    dbg.nospace() << "   showCross("
                  << inf.showCross << ")";

    return dbg.space();
}

} // namespace Digikam
