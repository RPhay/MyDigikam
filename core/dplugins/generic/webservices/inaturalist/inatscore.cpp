/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a tool to export images to iNaturalist web service
 *
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "inatscore.h"

// Qt includes

#include <QHash>
#include <QVector>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericINatPlugin
{

class Q_DECL_HIDDEN ComputerVisionScore::Private
{
public:

    Private()
        : frequencyScore(0.0),
          visionScore   (0.0),
          combinedScore (0.0)
    {
    }

    double frequencyScore;
    double visionScore;
    double combinedScore;
    Taxon  taxon;
};


ComputerVisionScore::ComputerVisionScore(double frequencyScore,
                                         double visionScore,
                                         double combinedScore,
                                         const Taxon& taxon)
    : d(new Private)
{
    d->frequencyScore = frequencyScore;
    d->visionScore    = visionScore;
    d->combinedScore  = combinedScore;
    d->taxon          = taxon;
}

ComputerVisionScore::ComputerVisionScore()
    : d(new Private)
{
}

ComputerVisionScore::ComputerVisionScore(const ComputerVisionScore& other)
    : d(new Private)
{
    *d = *other.d;
}

ComputerVisionScore& ComputerVisionScore::operator=(const ComputerVisionScore& other)
{
    *d = *other.d;

    return *this;
}

ComputerVisionScore::~ComputerVisionScore()
{
    delete d;
}

bool ComputerVisionScore::isValid() const
{
    return d->taxon.isValid();
}

double ComputerVisionScore::getFrequencyScore() const
{
    return d->frequencyScore;
}

double ComputerVisionScore::getVisionScore() const
{
    return d->visionScore;
}

double ComputerVisionScore::getCombinedScore() const
{
    return d->combinedScore;
}

const Taxon& ComputerVisionScore::getTaxon() const
{
    return d->taxon;
}

bool ComputerVisionScore::visuallySimilar() const
{
    return (d->visionScore > 0.0);
}

bool ComputerVisionScore::seenNearby() const
{
    return (d->frequencyScore > 0.0);
}

} // namespace DigikamGenericINatPlugin
