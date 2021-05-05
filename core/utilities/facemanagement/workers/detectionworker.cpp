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

    emit processed(package);
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
