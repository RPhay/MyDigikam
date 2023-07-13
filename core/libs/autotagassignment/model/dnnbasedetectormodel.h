/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Base class to perform low-level neural network inference
 *               for object detection
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_AUTOTAGS_DNN_BASE_DETECTOR_MODEL_H
#define DIGIKAM_AUTOTAGS_DNN_BASE_DETECTOR_MODEL_H

// C++ includes

#include <vector>

// Qt includes

#include <QMap>
#include <QVector>
#include <QString>
#include <QRect>
#include <QMutex>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNBaseDetectorModel
{

public:

    explicit DNNBaseDetectorModel();
    explicit DNNBaseDetectorModel(float scale, const cv::Scalar& val, const cv::Size& inputImgSize);
    virtual ~DNNBaseDetectorModel();

    virtual void detectObjects(const cv::Mat& inputImage,
                              QMap<QString, QVector<QRect>>& detectedBoxes) = 0;

    double showInferenceTime();

public:

    static float confidenceThreshold;    ///< Threshold for bbox detection. It can be init and changed in the GUI
    static float nmsThreshold;           ///< Threshold for nms suppression
    static float scoreThreshold;         ///< Threshold for class detection score

protected:

    float        scaleFactor;
    cv::Scalar   meanValToSubtract;
    cv::Size     inputImageSize;

    cv::dnn::Net net;
    QMutex       mutex;

private:

    // Disable
    DNNBaseDetectorModel(const DNNBaseDetectorModel&)            = delete;
    DNNBaseDetectorModel& operator=(const DNNBaseDetectorModel&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_AUTOTAGS_DNN_BASE_DETECTOR_MODEL_H
