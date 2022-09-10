/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Face detection benchmarker
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DETECTION_BENCH_MARKER_H
#define DIGIKAM_DETECTION_BENCH_MARKER_H

// Local includes

#include "facepipeline_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN DetectionBenchmarker : public WorkerObject
{
    Q_OBJECT

public:

    explicit DetectionBenchmarker(FacePipeline::Private* const d);
    QString result() const;

public Q_SLOTS:

    void process(FacePipelineExtendedPackage::Ptr package);

Q_SIGNALS:

    void processed(FacePipelineExtendedPackage::Ptr package);

protected:

    int                          totalImages;
    int                          faces;
    double                       totalPixels;
    double                       facePixels;

    int                          trueNegativeImages;
    int                          falsePositiveImages;

    int                          truePositiveFaces;
    int                          falseNegativeFaces;
    int                          falsePositiveFaces;

    FacePipeline::Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DETECTION_BENCH_MARKER_H
