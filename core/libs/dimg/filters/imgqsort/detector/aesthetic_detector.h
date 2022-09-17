/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - Aesthetic detection
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_AESTHETIC_DETECTOR_H
#define DIGIKAM_AESTHETIC_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"
#include "detector.h"

namespace Digikam
{

class AestheticDetector : public DetectorDistortion
{
    Q_OBJECT

public:

    explicit AestheticDetector();
    ~AestheticDetector();

    float detect(const cv::Mat& image)                          const override;

private:

    // Disable
    explicit AestheticDetector(QObject*);

    cv::Mat preprocess(const cv::Mat& image)                    const;
    float postProcess(const cv::Mat& modelOutput)               const;

public:

    static cv::dnn::Net model;
    static bool loadModel();
    static void unloadModel();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_AESTHETIC_DETECTOR_H
