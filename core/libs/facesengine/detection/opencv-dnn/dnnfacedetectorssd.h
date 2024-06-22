/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform SSD neural network inference
 *               for face detection
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

class DIGIKAM_EXPORT DNNFaceDetectorSSD: public DNNFaceDetectorBase
{

public:

    explicit DNNFaceDetectorSSD();
    ~DNNFaceDetectorSSD()                                         override = default;

    bool loadModels();

    void detectFaces(const cv::Mat& inputImage,
                     const cv::Size& paddedSize,
                     std::vector<cv::Rect>& detectedBboxes)       override;

private:

    void postprocess(cv::Mat detectionMat,
                     const cv::Size& paddedSize,
                     std::vector<cv::Rect>& detectedBboxes) const;

private:

    // Disable
    DNNFaceDetectorSSD(const DNNFaceDetectorSSD&)            = delete;
    DNNFaceDetectorSSD& operator=(const DNNFaceDetectorSSD&) = delete;
};

} // namespace Digikam
