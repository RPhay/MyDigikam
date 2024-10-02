/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2024-09-21
 * Description : Derived class to perform YuNet neural network inference
 *               for face detection. Credit: Shiqi yu for YuNet
 *               More information with YuNet:
 *               https://github.com/opencv/opencv_zoo/tree/main/models/face_detection_yunet
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QMutex>

// Local includes

#include "dnnfacedetectorbase.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNFaceDetectorYuNet: public DNNFaceDetectorBase
{
public:

    explicit DNNFaceDetectorYuNet();
    ~DNNFaceDetectorYuNet()                                                         override = default;

    bool loadModels();

    void detectFaces(const cv::Mat& inputImage,
                     const cv::Size& paddedSize,
                     std::vector<cv::Rect>& detectedBboxes)                         override;

    virtual void setFaceDetectionSize(FaceScanSettings::FaceDetectionSize faceSize) override;

protected:

    cv::Ptr<cv::FaceDetectorYN> cv_model;                   ///< The YuNet DNN model.
    static QMutex               lockModel;                  ///< Mutex for single-threading calls to the model.

private:

    std::vector<cv::String> getOutputsNames() const;

    cv::Mat callModel(const cv::Mat& inputImage);

private:

    // Disable
    DNNFaceDetectorYuNet(const DNNFaceDetectorYuNet&)            = delete;
    DNNFaceDetectorYuNet& operator=(const DNNFaceDetectorYuNet&) = delete;
};

} // namespace Digikam
