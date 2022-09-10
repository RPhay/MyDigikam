/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

    Q_EMIT processed(package);
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
