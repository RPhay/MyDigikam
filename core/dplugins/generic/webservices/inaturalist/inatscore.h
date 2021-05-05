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

#ifndef DIGIKAM_INAT_SCORE_H
#define DIGIKAM_INAT_SCORE_H

// Qt includes

#include <QList>
#include <QPair>

// Local includes

#include "inattaxon.h"

namespace DigikamGenericINatPlugin
{

class ComputerVisionScore
{
public:

    ComputerVisionScore();

    ComputerVisionScore(double frequencyStore, double visionScore,
                        double combinedScore, const Taxon& taxon);

    ComputerVisionScore(const ComputerVisionScore&);

    ~ComputerVisionScore();

    ComputerVisionScore& operator=(const ComputerVisionScore&);

    bool isValid()              const;

    bool visuallySimilar()      const;

    bool seenNearby()           const;

    double getFrequencyScore()  const;

    double getVisionScore()     const;

    double getCombinedScore()   const;

    const Taxon& getTaxon()     const;

private:

    class Private;
    Private* const d;

    class Node;
};

typedef QPair<QString, QList<ComputerVisionScore> > ImageScores;

} // namespace DigikamGenericINatPlugin

#endif // DIGIKAM_INAT_SCORE_H
