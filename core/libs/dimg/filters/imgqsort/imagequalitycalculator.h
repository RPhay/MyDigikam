/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 25/08/2013
 * Description : Image Quality Calculor
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#ifndef DIGIKAM_IMAGE_QUALITY_CALCULATOR_H
#define DIGIKAM_IMAGE_QUALITY_CALCULATOR_H

// Local includes 

#include "digikam_globals.h"

// Qt includes

#include <QString>
#include <QList>

namespace Digikam
{

class ImageQualityCalculator
{
public:

    struct ResultDetection
    {
        QString detetionType;
        float weight;
        float score;    
    };

public:

    explicit ImageQualityCalculator();
    ~ImageQualityCalculator();

    float calculateQuality()  const;
    void addDetectionResult(const QString& name, const float score, const float weight) const;

private:

    void normalizeWeight() const;
    void adjustWeightByQualityLevel() const;
    int numberDetectors() const;

private:

    class Private;
    Private* const d;
};

} 

#endif // DIGIKAM_IMAGE_QUALITY_CALCULATOR_H
