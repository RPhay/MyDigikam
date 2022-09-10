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

#include "detectionworker.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

DetectionWorker::DetectionWorker(FacePipeline::Private* const dd)
    : d(dd)
{
}

DetectionWorker::~DetectionWorker()
{
    wait();    // protect detector
}

void DetectionWorker::process(FacePipelineExtendedPackage::Ptr package)
{
    if (!package->image.isNull())
    {
/*
        QImage detectionImage  = scaleForDetection(package->image);
        package->detectedFaces = detector.detectFaces(detectionImage, package->image.originalSize());
*/
        package->detectedFaces = detector.detectFaces(package->image);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Found" << package->detectedFaces.size() << "faces in"
                                     << package->info.name() << package->image.size()
                                     << package->image.originalSize();
    }

    package->processFlags |= FacePipelinePackage::ProcessedByDetector;

    Q_EMIT processed(package);
}

QImage DetectionWorker::scaleForDetection(const DImg& image) const
{
    int recommendedSize = detector.recommendedImageSize(image.size());

    if (qMax(image.width(), image.height()) > (uint)recommendedSize)
    {
        return image.smoothScale(recommendedSize, recommendedSize, Qt::KeepAspectRatio).copyQImage();
    }

    return image.copyQImage();
}

void DetectionWorker::setAccuracyAndModel(double accuracy, bool yolo)
{
    QVariantMap params;
    params[QLatin1String("accuracy")]    = accuracy;
    params[QLatin1String("useyolov3")]   = yolo;
    params[QLatin1String("specificity")] = 0.8;     // TODO: add UI for sensitivity - specificity
    detector.setParameters(params);
}

} // namespace Digikam
