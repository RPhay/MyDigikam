/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-07-22
 * Description : Class to perform faces detection using OpenCV DNN module
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FACESENGINE_OPENCV_DNN_FACE_DETECTOR_H
#define DIGIKAM_FACESENGINE_OPENCV_DNN_FACE_DETECTOR_H

// Qt includes

#include <QImage>
#include <QList>
#include <QRect>

// Local includes

#include "digikam_opencv.h"
#include "dimg.h"
#include "dnnfacedetectorbase.h"

namespace Digikam
{

enum DetectorNNModel
{
    SSDMOBILENET = 0,   /// SSD MobileNet neural network inference.
    YOLO                /// YOLO neural network inference.
};

class OpenCVDNNFaceDetector
{

public:

    explicit OpenCVDNNFaceDetector(DetectorNNModel model = DetectorNNModel::SSDMOBILENET);
    ~OpenCVDNNFaceDetector();

    cv::Mat prepareForDetection(const DImg& inputImage, cv::Size& paddedSize)        const;
    cv::Mat prepareForDetection(const QImage& inputImage, cv::Size& paddedSize)      const;
    cv::Mat prepareForDetection(const QString& inputImagePath, cv::Size& paddedSize) const;

    QList<QRect> detectFaces(const cv::Mat& inputImage, const cv::Size& paddedSize);
    std::vector<cv::Rect> cvDetectFaces(const cv::Mat& inputImage, const cv::Size& paddedSize);

    /**
     * Returns the image size (one dimension)
     * recommended for face detection. If the image is considerably larger, it will be rescaled automatically.
     */
    static int recommendedImageSizeForDetection();

private:

    cv::Mat prepareForDetection(cv::Mat& cvImage, cv::Size& paddedSize) const;

private:

    OpenCVDNNFaceDetector(const OpenCVDNNFaceDetector&);            // Disable
    OpenCVDNNFaceDetector& operator=(const OpenCVDNNFaceDetector&); // Disable

private:

    DetectorNNModel      m_modelType;
    DNNFaceDetectorBase* m_inferenceEngine;
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_OPENCV_DNN_FACE_DETECTOR_H
