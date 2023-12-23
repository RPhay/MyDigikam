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

#ifndef DIGIKAM_MARBLE_SETTINGS_CONTAINER_H
#define DIGIKAM_MARBLE_SETTINGS_CONTAINER_H

// Qt includes

#include <QDebug>

// Local includes

#include "MarbleGlobal.h"
#include "MarbleLocale.h"
#include "digikam_export.h"

class KConfigGroup;

using namespace Marble;

namespace Digikam
{

/**
 * The class MarbleSettingsContainer encapsulates all Marble related settings.
 */
class DIGIKAM_EXPORT MarbleSettingsContainer
{
public:

    MarbleSettingsContainer()  = default;
    ~MarbleSettingsContainer() = default;

public:

    void readFromConfig(KConfigGroup& group);
    void writeToConfig(KConfigGroup& group) const;

public:

    MarbleLocale::MeasurementSystem measurementSys           = MarbleLocale::MetricSystem;

    Marble::AngleUnit               angleUnit                = Marbleglobal::DecimalDegree;

    Marble::MapQuality              stillQ                   = MarbleGlobal::HighQuality;
    Marble::MapQuality              animationQ               = MarbleGlobal::LowQuality;

    QFont                           mapFont;

    bool                            inertialRotation         = true;
    bool                            mouseRotation            = true;

    int                             volatileTileCacheLimit   = 100;
    int                             persistentTileCacheLimit = 999999;
};

//! qDebug() stream operator. Writes property @a inf to the debug output in a nicely formatted way.
DIGIKAM_EXPORT QDebug operator<<(QDebug dbg, const MarbleSettingsContainer& inf);

} // namespace Digikam

Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::MarbleSettingsContainer::RotationBehaviorFlags)

#endif // DIGIKAM_MARBLE_SETTINGS_CONTAINER_H
