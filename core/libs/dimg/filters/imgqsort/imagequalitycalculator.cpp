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

#include "imagequalitycalculator.h"

// Local includes

// Qt includes

#include <QList>
#include <QDebug>

namespace Digikam
{

class Q_DECL_HIDDEN ImageQualityCalculator::Private
{
public:

    explicit Private()
      : threshold_punish(0.9),
        weight_punish(20.0)
    {
    }

    float threshold_punish;
    float weight_punish;

    QList<ResultDetection> detectionResults;
};

ImageQualityCalculator::ImageQualityCalculator()
    : d(new Private)
{
    d->detectionResults = QList<ResultDetection>();
}

ImageQualityCalculator::~ImageQualityCalculator()
{
    delete d;
}


void ImageQualityCalculator::addDetectionResult(const QString& name, const float score, const float weight) const
{
    ResultDetection result;

    result.detetionType = name;
    result.weight       = weight;
    result.score        = score;

    d->detectionResults.append(result);
}

void ImageQualityCalculator::normalizeWeight() const
{
    float sum = 0;

    for (const auto result : d->detectionResults)
    {
        sum += result.weight;
    }

    for (auto& result : d->detectionResults)
    {
        result.weight /= sum;
    }
}

float ImageQualityCalculator::calculateQuality() const
{
    if (!numberDetectors())
    {
        return -1;
    }

    adjustWeightByQualityLevel();

    normalizeWeight();

    float damage = 0.0;

    for (const auto result : d->detectionResults)
    {
        damage += result.score * result.weight;
    }

    return (1 - damage)*100;
}

int ImageQualityCalculator::numberDetectors() const
{
    return d->detectionResults.count();
}

void ImageQualityCalculator::adjustWeightByQualityLevel() const
{
    for (auto& result : d->detectionResults)
    {
        if (result.score > d->threshold_punish)
        {
            result.weight *= d->weight_punish;
        }
    }
}

}
