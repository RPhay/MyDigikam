/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2023-09-02
 * Description : Class for autotagging engine
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsassign.h"

// C++ includes

#include <vector>

// Qt includes

#include <QtGlobal>
#include <QStandardPaths>
#include <qmath.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_opencv.h"
#include "dnnyolodetector.h"
#include "dnnresnetdetector.h"

namespace Digikam
{

autoTagsAssign::autoTagsAssign(DetectorModel model)
    : m_modelType(model)
{
    switch (m_modelType)
    {
        case DetectorModel::YOLOV5NANO:
        {
            m_inferenceEngine = new DNNYoloDetector(YoloVersions(m_modelType));
            break;
        }

        case DetectorModel::YOLOV5XLARGE:
        {
            m_inferenceEngine = new DNNYoloDetector(YoloVersions(m_modelType));
            break;
        }

        case DetectorModel::RESNET50:
        {
            m_inferenceEngine = new DNNResnetDetector();
            break;
        }

        default:
        {
            qFatal("UNKNOWN neural network model");
        }
    }
}

autoTagsAssign::~autoTagsAssign()
{
    delete m_inferenceEngine;
}

cv::Mat autoTagsAssign::prepareForDetection(const DImg& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    cv::Mat cvImage;
    int type               = inputImage.sixteenBit() ? CV_16UC4 : CV_8UC4;
    cv::Mat cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), type, inputImage.bits());

    if (inputImage.hasAlpha())
    {
        cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGBA2BGR);
    }
    else
    {
        cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGB2BGR);
    }

    if (type == CV_16UC4)
    {
        cvImage.convertTo(cvImage, CV_8UC3, 1 / 256.0);
    }

    return cvImage;
}

cv::Mat autoTagsAssign::prepareForDetection(const QImage& inputImage) const
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

    return cvImage;
}

cv::Mat autoTagsAssign::prepareForDetection(const QString& inputImagePath) const
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

    return cvImage;
}

std::vector<cv::Mat> autoTagsAssign::prepareForDetection(const QList<QString>& inputImagePaths, int batchSize) const
{
    std::vector<cv::Mat> result;    
    for (auto imgPath : inputImagePaths)
    {
        result.push_back(prepareForDetection(imgPath));
    }

    // add black imgs to fullfill the batch size
    cv::Size inputSize = m_inferenceEngine->getinputImageSize();  
    while ((result.size() % batchSize) != 0)
    {   
        cv::Mat dummycvImg = cv::Mat::zeros(inputSize.height, inputSize.width, CV_8UC3);
        result.push_back(dummycvImg);
    }

    return result;
}

QList<QString> autoTagsAssign::generateTagsList(const QImage& image)
{
    QList<QString> result;

    if (image.isNull() || !image.size().isValid())
    {
        return result;
    }

    try
    {
        cv::Mat cvImage       = prepareForDetection(image);
        result = m_inferenceEngine->generateObjects(cvImage);
        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QString> autoTagsAssign::generateTagsList(const DImg& image)
{
    QList<QString> result;

    if (image.isNull() || !image.size().isValid())
    {
        return result;
    }

    try
    {
        cv::Mat cvImage       = prepareForDetection(image);
        result = m_inferenceEngine->generateObjects(cvImage);
        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QString> autoTagsAssign::generateTagsList(const QString& imagePath)
{
    QList<QString> result;

    try
    {
        cv::Mat cvImage       = prepareForDetection(imagePath);
        result = m_inferenceEngine->generateObjects(cvImage);
        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QList<QString>> autoTagsAssign::generateTagsList(const QList<QString>& inputImagePaths, int batchSize) const
{
    QList<QList<QString>> result;

    try
    {
        std::vector<cv::Mat> cvImages = prepareForDetection(inputImagePaths, batchSize);
        result = m_inferenceEngine->generateObjects(cvImages);
        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QString> autoTagsAssign::getPredefinedTagsPath() const
{
    QList<QString> tagsPaths;
    QList<QString> objects = m_inferenceEngine->getPredefinedClasses();

    QString rootTags = QLatin1String("auto/");
    for (auto obj : objects)
    {
        tagsPaths.append(rootTags + obj);
    }

    return tagsPaths;
}

} // namespace Digikam
