/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-07-22
 * Description : Class to perform faces detection using OpenCV DNN module
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

// Local includes

#include "dnnfacedetectorssd.h"
#include "dnnfacedetectoryolo.h"

// Qt includes

#include <QtGlobal>
#include <QStandardPaths>
#include <qmath.h>

// Local includes

#include "digikam_debug.h"

// OpenCV includes

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// C++ includes

#include <cassert>
#include <vector>

namespace Digikam
{

OpenCVDNNFaceDetector::OpenCVDNNFaceDetector(DetectorNNModel model)
  : modelType(model)
{
    switch(model)
    {
        case DetectorNNModel::SSDMOBILENET:
        {
            inferenceEngine = new DNNFaceDetectorSSD;
            break;
        }
        case DetectorNNModel::YOLO:
        {
            inferenceEngine = new DNNFaceDetectorYOLO;
            break;
        }
        default:
        {
            assert(0 && "UNKNOWN neural network model");
        }
    }
}

OpenCVDNNFaceDetector::~OpenCVDNNFaceDetector()
{
    delete inferenceEngine;
}

int OpenCVDNNFaceDetector::recommendedImageSizeForDetection()
{
    return 800;
}

cv::Mat OpenCVDNNFaceDetector::prepareForDetection(const QImage& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    QImage image(inputImage);
    qint64 inputArea                    = image.width() * image.height();
    const qint64 maxAcceptableInputArea = 1024*768;

    if (inputArea > maxAcceptableInputArea)
    {
        // Resize to 1024 * 768 (or comparable area for different aspect ratio)
        // Looking for scale factor z where A = w*z * h*z => z = sqrt(A/(w*h))
        qreal z          = qSqrt(qreal(maxAcceptableInputArea) / image.width() / image.height());
        QSize scaledSize = image.size() * z;
        image            = image.scaled(scaledSize, Qt::KeepAspectRatio);
    }

    cv::Mat cvImageWrapper, cvImage;

    switch (image.format())
    {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            // I think we can ignore premultiplication when converting to grayscale
            cvImageWrapper = cv::Mat(image.height(), image.width(), CV_8UC4, image.scanLine(0), image.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGBA2RGB);
            break;
        default:
            image          = image.convertToFormat(QImage::Format_RGB888);
            cvImage        = cv::Mat(image.height(), image.width(), CV_8UC3, image.scanLine(0), image.bytesPerLine());
            // cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGB2BGR);
            break;
    }

    // cv::equalizeHist(cvImage, cvImage);

    // cv::cvtColor(cvImage, cvImage, cv::COLOR_GRAY2BGR);

    return cvImage;
}

cv::Mat OpenCVDNNFaceDetector::prepareForDetection(const Digikam::DImg& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    Digikam::DImg image(inputImage);
    qint64 inputArea                    = image.width() * image.height();
    const qint64 maxAcceptableInputArea = 1024*768;

    if (inputArea > maxAcceptableInputArea)
    {
        // Resize to 1024 * 768 (or comparable area for different aspect ratio)
        // Looking for scale factor z where A = w*z * h*z => z = sqrt(A/(w*h))
        qreal z          = qSqrt(qreal(maxAcceptableInputArea) / image.width() / image.height());
        QSize scaledSize = image.size() * z;
        image            = image.smoothScale(scaledSize, Qt::KeepAspectRatio);
    }

    cv::Mat cvImageWrapper, cvImage;

    cvImageWrapper = cv::Mat(image.height(), image.width(), CV_8UC3, image.bits());
    cv::cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGB2BGR);

    // cv::equalizeHist(cvImage, cvImage);

    // cv::cvtColor(cvImage, cvImage, cv::COLOR_GRAY2BGR);

    return cvImage;
}

cv::Mat OpenCVDNNFaceDetector::prepareForDetection(const QString& inputImagePath, cv::Size& paddedSize) const
{
    cv::Mat image = cv::imread(inputImagePath.toStdString()), imagePadded;

    cv::Size inputImageSize = inferenceEngine->nnInputSizeRequired();
    float k = qMin(inputImageSize.width*1.0/image.cols, inputImageSize.height*1.0/image.rows);

    int newWidth = (int)(k*image.cols);
    int newHeight = (int)(k*image.rows);
    cv::resize(image, image, cv::Size(newWidth, newHeight));

    // Pad with black pixels
    int padX = (inputImageSize.width - newWidth) / 2;
    int padY = (inputImageSize.height - newHeight) / 2;
    cv::copyMakeBorder(image, imagePadded,
                       padY, padY,
                       padX, padX,
                       cv::BORDER_CONSTANT,
                       cv::Scalar(0,0,0));

    paddedSize = cv::Size(padX, padY);

    return imagePadded;
}

/** There is no proof that doing this will help, since face can be detected at various positions (even half, masked faces
 *  can be detected), not only frontal. Effort on doing this should be questioned. 
void OpenCVDNNFaceDetector::resizeBboxToStandardHumanFace(int& width, int& height)
>>>>>>> Restructure and improve Face Detection module
{
    // Human head sizes data
    // https://en.wikipedia.org/wiki/Human_head#Average_head_sizes

    float maxRatioFrontalFace = 15.4 / 15.5;
    float minRatioNonFrontalFace = 8.6 / 21.6;

    float r = width*1.0/height, rReference;

    if((r >= minRatioNonFrontalFace*0.9 && r <= maxRatioFrontalFace*1.1))
    {
        rReference = r;
    }
    else if(r <= 0.25)
    {
        rReference = r*1.5; 
    }
    else if(r >= 4)
    {
        rReference = r/1.5; 
    }
    else if(r < minRatioNonFrontalFace*0.9)
    {
        rReference = minRatioNonFrontalFace;
    }
    else if(r > maxRatioFrontalFace*1.1)
    {
        rReference = maxRatioFrontalFace;
    }

    if(width > height)
    {
        height = width / rReference;
    }
    else
    {
        width = height * rReference;
    }
}
*/

QList<QRect> OpenCVDNNFaceDetector::detectFaces(const cv::Mat& inputImage, const cv::Size& paddedSize)
{
    std::vector<cv::Rect> detectedBboxes;
    inferenceEngine->detectFaces(inputImage, paddedSize, detectedBboxes);

    QList<QRect> results;

    // cv::Mat imageTest = inputImage.clone();

    for(cv::Rect bbox : detectedBboxes)
    {
        QRect rect(bbox.x, bbox.y, bbox.width, bbox.height);
        results << rect;
        // qDebug() << rect;
        // cv::rectangle(imageTest, cv::Rect(bbox.x + paddedSize.width,
        //                                   bbox.y + paddedSize.height,
        //                                   bbox.width, bbox.height), cv::Scalar(0,128,0));
    }

    // cv::imshow("image", imageTest);
    // cv::waitKey(0);

    return results;
}

} // namespace Digikam
