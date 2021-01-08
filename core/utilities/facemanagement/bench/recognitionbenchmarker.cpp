/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Face recognition benchmarker
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "recognitionbenchmarker.h"

// Local includes

#include "digikam_debug.h"
#include "tagscache.h"

namespace Digikam
{

RecognitionBenchmarker::Statistics::Statistics()
    : knownFaces         (0),
      correctlyRecognized(0)
{
}

RecognitionBenchmarker::RecognitionBenchmarker(FacePipeline::Private* const d)
    : d(d)
{
}

/**
 * NOTE: Bench performance code. No need i18n here
 */
QString RecognitionBenchmarker::result() const
{
    int totalImages = 0;

    foreach (const Statistics& stat, results)
    {
        totalImages += stat.knownFaces;
    }

    QString s = QString::fromUtf8("<p>"
                        "<u>Collection Properties:</u><br/>"
                        "%1 Images <br/>"
                        "%2 Identities <br/>"
                        "</p><p>").arg(totalImages).arg(results.size());

    for (QMap<int, Statistics>::const_iterator it = results.begin() ;
         it != results.end() ; ++it)
    {
        const Statistics& stat = it.value();
        double correctRate     = double(stat.correctlyRecognized) / stat.knownFaces;
        s                     += TagsCache::instance()->tagName(it.key());
        s                     += QString::fromUtf8(": %1 faces, %2 (%3%) correctly recognized<br/>")
                                 .arg(stat.knownFaces).arg(stat.correctlyRecognized).arg(correctRate * 100);
    }

    s += QLatin1String("</p>");

    return s;
}

// TODO: investigate this method
void RecognitionBenchmarker::process(FacePipelineExtendedPackage::Ptr package)
{
    FaceUtils utils;

    for (int i = 0 ; i < package->databaseFaces.size() ; ++i)
    {
        Identity identity  = utils.identityForTag(package->databaseFaces[i].tagId(), recognizer);
        Statistics& result = results[package->databaseFaces[i].tagId()];
        result.knownFaces++;

        if (identity == package->recognitionResults[i])
        {
            result.correctlyRecognized++;
        }
    }

    emit processed(package);
}

} // namespace Digikam
