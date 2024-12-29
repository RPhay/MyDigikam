/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
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
      d(dd)
{
}

RecognitionWorker::~RecognitionWorker()
{
    wait();    // Protect the database.
}

/**
 * TODO: investigate this method.
 */
void RecognitionWorker::process(const FacePipelineExtendedPackage::Ptr& package)
{
    FaceUtils      utils;
    QList<QImage*> images;

    if (package->processFlags & FacePipelinePackage::ProcessedByDetector)
    {
        // Assume we have an image.

        images = imageRetriever.getDetails(package->image,
                                           package->detectedFaces);
    }

    else if (!package->databaseFaces.isEmpty())
    {
        images = imageRetriever.getThumbnails(package->filePath,
                                              package->databaseFaces.toFaceTagsIfaceList());
    }

    if (package->image.isNull() && (images.size() > 0))
    {
        package->image              = DImg(*images[0]);
        package->processedFaceCount = images.size();
    }

    // NOTE: cropped faces will be deleted by training provider.

    package->recognitionResults = recognizer.recognizeFaces(images);
    package->processFlags      |= FacePipelinePackage::ProcessedByRecognizer;

    Q_EMIT processed(package);
}

void RecognitionWorker::setThreshold(int threshold, bool)
{
    recognizer.setParameter(QLatin1String("recognizeAccuracy"), threshold);
}

void RecognitionWorker::aboutToDeactivate()
{
    imageRetriever.cancel();
}

void RecognitionWorker::setAccuracyAndModel(int detectAccuracy,
                                            FaceScanSettings::FaceDetectionModel detectModel,
                                            FaceScanSettings::FaceDetectionSize detectSize,
                                            int recognizeAccuracy,
                                            FaceScanSettings::FaceRecognitionModel recognizeModel)
{

    QVariantMap params;
    params[QLatin1String("detectAccuracy")]       = detectAccuracy;
    params[QLatin1String("detectModel")]          = detectModel;
    params[QLatin1String("detectSize")]           = detectSize;
    params[QLatin1String("recognizeAccuracy")]    = recognizeAccuracy;
    params[QLatin1String("recognizeModel")]       = recognizeModel;
    recognizer.setParameters(params);
    /*
        recognizer.setParameter(QLatin1String("detectAccuracy"), detectAccuracy);
        recognizer.setParameter(QLatin1String("detectModel"), detectModel);
        recognizer.setParameter(QLatin1String("detectSize"), detectSize);
        recognizer.setParameter(QLatin1String("recognizeAccuracy"), recognizeAccuracy);
        recognizer.setParameter(QLatin1String("recognizeModel"), recognizeModel);

        Q_EMIT d->accuracyAndModel(detectAccuracy, detectModel, detectSize, recognizeAccuracy, recognizeModel);
    */
}

} // namespace Digikam

#include "moc_recognitionworker.cpp"
