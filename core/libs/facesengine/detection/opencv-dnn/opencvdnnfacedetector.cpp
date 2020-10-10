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

#include "opencvdnnfacedetector.h"

// C++ includes

#include <vector>

// Qt includes

#include <QtGlobal>
#include <QStandardPaths>
#include <qmath.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_opencv.h"
#include "dnnfacedetectorssd.h"
#include "dnnfacedetectoryolo.h"

namespace Digikam
{

OpenCVDNNFaceDetector::OpenCVDNNFaceDetector(DetectorNNModel model)
    : m_modelType(model)
{
    switch (m_modelType)
    {
        case DetectorNNModel::SSDMOBILENET:
        {
            m_inferenceEngine = new DNNFaceDetectorSSD;
            break;
        }

        case DetectorNNModel::YOLO:
        {
            m_inferenceEngine = new DNNFaceDetectorYOLO;
            break;
        }

        default:
        {
            qFatal("UNKNOWN neural network model");
        }
    }
}

OpenCVDNNFaceDetector::~OpenCVDNNFaceDetector()
{
    delete m_inferenceEngine;
}

int OpenCVDNNFaceDetector::recommendedImageSizeForDetection()
{
    return 800;
}

// TODO: prepareForDetection give different performances
cv::Mat OpenCVDNNFaceDetector::prepareForDetection(const DImg& inputImage, cv::Size& paddedSize) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    cv::Mat cvImage;
    cv::Mat cvImageWrapper;
    int type = inputImage.sixteenBit() ? CV_16UC3 : CV_8UC3;
    type     = inputImage.hasAlpha()   ? type     : type + 8;

    switch (type)
    {
        case CV_8UC4:
        case CV_16UC4:
        {
            cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), type, inputImage.bits());
            cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGBA2BGR);
            break;
        }

        case CV_8UC3:
        case CV_16UC3:
        {
            cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), type, inputImage.bits());
            cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGB2BGR);
            break;
        }
    }

    return prepareForDetection(cvImage, paddedSize);
}

cv::Mat OpenCVDNNFaceDetector::prepareForDetection(const QImage& inputImage, cv::Size& paddedSize) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    cv::Mat cvImage;
    cv::Mat cvImageWrapper;
    QImage qimage(inputImage);

    switch (qimage.format())
    {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
        {
            // I think we can ignore premultiplication when converting to grayscale

            cvImageWrapper = cv::Mat(qimage.height(), qimage.width(), CV_8UC4,
                                     qimage.scanLine(0), qimage.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGBA2BGR);
            break;
        }

        default:
        {
            qimage         = qimage.convertToFormat(QImage::Format_RGB888);
            cvImageWrapper = cv::Mat(qimage.height(), qimage.width(), CV_8UC3,
                                     qimage.scanLine(0), qimage.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGB2BGR);
            break;
        }
    }

    return prepareForDetection(cvImage, paddedSize);
}

cv::Mat OpenCVDNNFaceDetector::prepareForDetection(const QString& inputImagePath, cv::Size& paddedSize) const
{
    std::vector<char> buffer;
    QFile file(inputImagePath);
    buffer.resize(file.size());

    if (!file.open(QIODevice::ReadOnly))
    {
        return cv::Mat();
    }

    file.read(buffer.data(), file.size());
    file.close();

    cv::Mat cvImage = cv::imdecode(std::vector<char>(buffer.begin(), buffer.end()), cv::IMREAD_COLOR);

    return prepareForDetection(cvImage, paddedSize);
}

cv::Mat OpenCVDNNFaceDetector::prepareForDetection(cv::Mat& cvImage, cv::Size& paddedSize) const
{
    // Resize image before padding to fit in neural net

    cv::Size inputImageSize = m_inferenceEngine->nnInputSizeRequired();
    float k                 = qMin(inputImageSize.width  * 1.0 / cvImage.cols,
                                   inputImageSize.height * 1.0 / cvImage.rows);

    int newWidth            = (int)(k * cvImage.cols);
    int newHeight           = (int)(k * cvImage.rows);
    cv::resize(cvImage, cvImage, cv::Size(newWidth, newHeight));

    // Pad with black pixels

    int padX                = (inputImageSize.width  - newWidth)  / 2;
    int padY                = (inputImageSize.height - newHeight) / 2;

    cv::Mat imagePadded;

    cv::copyMakeBorder(cvImage, imagePadded,
                       padY, padY,
                       padX, padX,
                       cv::BORDER_CONSTANT,
                       cv::Scalar(0, 0, 0));

    paddedSize              = cv::Size(padX, padY);

    return imagePadded;
}

/**
 * There is no proof that doing this will help, since face can be detected at various positions (even half, masked faces
 * can be detected), not only frontal. Effort on doing this should be questioned.
 * TODO: Restructure and improve Face Detection module.

void OpenCVDNNFaceDetector::resizeBboxToStandardHumanFace(int& width, int& height)
{
    // Human head sizes data
    // https://en.wikipedia.org/wiki/Human_head#Average_head_sizes

    float maxRatioFrontalFace    = 15.4 / 15.5;
    float minRatioNonFrontalFace = 8.6  / 21.6;

    float r = width*1.0/height, rReference;

    if      ((r >= minRatioNonFrontalFace*0.9) && r <= (maxRatioFrontalFace * 1.1))
    {
        rReference = r;
    }
    else if (r <= 0.25)
    {
        rReference = r * 1.5;
    }
    else if (r >= 4)
    {
        rReference = r / 1.5;
    }
    else if (r < minRatioNonFrontalFace * 0.9)
    {
        rReference = minRatioNonFrontalFace;
    }
    else if (r > maxRatioFrontalFace * 1.1)
    {
        rReference = maxRatioFrontalFace;
    }

    if (width > height)
    {
        height = width / rReference;
    }
    else
    {
        width = height * rReference;
    }
}
*/

QList<QRect> OpenCVDNNFaceDetector::detectFaces(const cv::Mat& inputImage,
                                                const cv::Size& paddedSize)
{
    std::vector<cv::Rect> detectedBboxes = cvDetectFaces(inputImage, paddedSize);

    QList<QRect> results;
/*
    cv::Mat imageTest = inputImage.clone();
*/
    for (const cv::Rect& bbox : detectedBboxes)
    {
        QRect rect(bbox.x, bbox.y, bbox.width, bbox.height);
        results << rect;
/*
        qCDebug(DIGIKAM_FACESENGINE_LOG) << rect;
        cv::rectangle(imageTest, cv::Rect(bbox.x + paddedSize.width,
                                          bbox.y + paddedSize.height,
                                          bbox.width, bbox.height), cv::Scalar(0, 128, 0));
*/
    }
/*
    cv::imshow("image", imageTest);
    cv::waitKey(0);
*/
    return results;
}

std::vector<cv::Rect> OpenCVDNNFaceDetector::cvDetectFaces(const cv::Mat& inputImage,
                                                           const cv::Size& paddedSize)
{
    std::vector<cv::Rect> detectedBboxes;

    m_inferenceEngine->detectFaces(inputImage, paddedSize, detectedBboxes);

    return detectedBboxes;
}

} // namespace Digikam
