/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Base class to perform low-level neural network inference
 *               for face detection
 *
 * SPDX-FileCopyrightText: 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_BASE_H
#define DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_BASE_H

// C++ includes

#include <vector>

// Qt includes

#include <QMutex>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNFaceDetectorBase
{

public:

    DNNFaceDetectorBase()          = default;
    explicit DNNFaceDetectorBase(float scale, const cv::Scalar& val, const cv::Size& inputImgSize);
    virtual ~DNNFaceDetectorBase() = default;

    virtual void detectFaces(const cv::Mat& inputImage,
                             const cv::Size& paddedSize,
                             std::vector<cv::Rect>& detectedBboxes) = 0;

    cv::Size nnInputSizeRequired() const;

protected:

    void selectBbox(const cv::Size& paddedSize,
                    float confidence,
                    int left,
                    int right,
                    int top,
                    int bottom,
                    std::vector<float>& goodConfidences, std::vector<cv::Rect>& goodBoxes,
                    std::vector<float>& doubtConfidences, std::vector<cv::Rect>& doubtBoxes) const;

    void correctBbox(cv::Rect& bbox,
                     const cv::Size& paddedSize) const;

public:

    static float confidenceThreshold;    ///< Threshold for bbox detection. It can be init and changed in the GUI
    static float nmsThreshold;           ///< Threshold for nms suppression

protected:

    float        scaleFactor        = 1.0F;
    cv::Scalar   meanValToSubtract;
    cv::Size     inputImageSize;

    cv::dnn::Net net;

    QMutex       mutex;

private:

    // Disable
    DNNFaceDetectorBase(const DNNFaceDetectorBase&)            = delete;
    DNNFaceDetectorBase& operator=(const DNNFaceDetectorBase&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_BASE_H
