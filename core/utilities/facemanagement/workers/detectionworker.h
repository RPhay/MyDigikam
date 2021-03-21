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

#ifndef DIGIKAM_DETECTION_WORKER_H
#define DIGIKAM_DETECTION_WORKER_H

// Local includes

#include "facepipeline_p.h"
#include "faceitemretriever.h"

namespace Digikam
{

class Q_DECL_HIDDEN DetectionWorker : public WorkerObject
{
    Q_OBJECT

public:

    explicit DetectionWorker(FacePipeline::Private* const dd);
    ~DetectionWorker() override;

    QImage scaleForDetection(const DImg& image) const;

public Q_SLOTS:

    void process(FacePipelineExtendedPackage::Ptr package);
    void setAccuracyAndModel(double value, bool yolo);

Q_SIGNALS:

    void processed(FacePipelineExtendedPackage::Ptr package);

protected:

    FaceDetector                 detector;
    FacePipeline::Private* const d;

private:

    // Disable
    DetectionWorker(const DetectionWorker&)            = delete;
    DetectionWorker& operator=(const DetectionWorker&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_DETECTION_WORKER_H
