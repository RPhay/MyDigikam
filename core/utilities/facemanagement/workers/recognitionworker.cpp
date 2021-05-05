/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
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

#include "recognitionworker.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

RecognitionWorker::RecognitionWorker(FacePipeline::Private* const dd)
    : imageRetriever(dd),
      d             (dd)
{
}

RecognitionWorker::~RecognitionWorker()
{
    wait();    // protect database
}

/**
 *TODO: investigate this method
 */
void RecognitionWorker::process(FacePipelineExtendedPackage::Ptr package)
{
    FaceUtils      utils;
    QList<QImage*> images;

    if      (package->processFlags & FacePipelinePackage::ProcessedByDetector)
    {
        // assume we have an image

        images = imageRetriever.getDetails(package->image, package->detectedFaces);
    }
    else if (!package->databaseFaces.isEmpty())
    {
        images = imageRetriever.getThumbnails(package->filePath, package->databaseFaces.toFaceTagsIfaceList());
    }

    // NOTE: cropped faces will be deleted by training provider

    package->recognitionResults  = recognizer.recognizeFaces(images);
    package->processFlags       |= FacePipelinePackage::ProcessedByRecognizer;

    emit processed(package);
}

void RecognitionWorker::setThreshold(double threshold, bool)
{
    recognizer.setParameter(QLatin1String("threshold"), threshold);
}

void RecognitionWorker::aboutToDeactivate()
{
    imageRetriever.cancel();
}

} // namespace Digikam
