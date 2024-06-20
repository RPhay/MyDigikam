/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

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

    void process(const FacePipelineExtendedPackage::Ptr& package);
    void setAccuracyAndModel(double value, bool yolo);

Q_SIGNALS:

    void processed(const FacePipelineExtendedPackage::Ptr& package);

protected:

    FaceDetector                 detector;
    FacePipeline::Private* const d          = nullptr;

private:

    // Disable
    DetectionWorker(const DetectionWorker&)            = delete;
    DetectionWorker& operator=(const DetectionWorker&) = delete;
};

} // namespace Digikam
