/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform YOLO neural network inference
 *               for face detection. Credit: Ayoosh Kathuria (for Yolov3 blog post),
 *               sthanhng (for example of face detection with Yolov3).
 *               More information with Yolov3:
 *               https://towardsdatascience.com/yolo-v3-object-detection-53fb7d3bfe6b
 *               sthanhng github on face detection with Yolov3:
 *               https://github.com/sthanhng/yoloface
 *
 * SPDX-FileCopyrightText: 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "dnnfacedetectorbase.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNFaceDetectorYOLO: public DNNFaceDetectorBase
{

public:

    explicit DNNFaceDetectorYOLO();
    ~DNNFaceDetectorYOLO()                                        override = default;

    bool loadModels();

    void detectFaces(const cv::Mat& inputImage,
                     const cv::Size& paddedSize,
                     std::vector<cv::Rect>& detectedBboxes)       override;

private:

    std::vector<cv::String> getOutputsNames()               const;

    void postprocess(const std::vector<cv::Mat>& outs,
                     const cv::Size& paddedSize,
                     std::vector<cv::Rect>& detectedBboxes) const;

private:

    // Disable
    DNNFaceDetectorYOLO(const DNNFaceDetectorYOLO&)            = delete;
    DNNFaceDetectorYOLO& operator=(const DNNFaceDetectorYOLO&) = delete;
};

} // namespace Digikam
