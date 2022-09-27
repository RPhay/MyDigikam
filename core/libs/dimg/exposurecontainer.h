/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-01-12
 * Description : exposure indicator settings container.
 *
 * SPDX-FileCopyrightText: 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EXPOSURE_CONTAINER_H
#define DIGIKAM_EXPOSURE_CONTAINER_H

// Qt includes

#include <QColor>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ExposureSettingsContainer
{

public:

    explicit ExposureSettingsContainer()
        : underExposureIndicator(false),
          overExposureIndicator (false),
          exposureIndicatorMode (true),
          underExposurePercent  (1.0),
          overExposurePercent   (1.0),
          underExposureColor    (Qt::white),
          overExposureColor     (Qt::black)
    {
    };

    virtual ~ExposureSettingsContainer()
    {
    };

public:

    bool   underExposureIndicator;
    bool   overExposureIndicator;

    /**
     * If this option is true, over and under exposure indicators will be displayed
     * only when pure white and pure black color matches, as all color components match
     * the condition in the same time.
     * Else indicators are turn on when one of color components match the condition.
     */
    bool   exposureIndicatorMode;

    float  underExposurePercent;
    float  overExposurePercent;

    QColor underExposureColor;
    QColor overExposureColor;

private:

    Q_DISABLE_COPY(ExposureSettingsContainer)
};

} // namespace Digikam

#endif // DIGIKAM_EXPOSURE_CONTAINER_H
