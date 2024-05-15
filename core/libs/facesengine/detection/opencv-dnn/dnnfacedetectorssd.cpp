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

#include "dnnfacedetectorssd.h"

// Qt includes

#include <QList>
#include <QRect>
#include <QString>
#include <QFileInfo>
#include <QMutexLocker>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"

namespace Digikam
{

DNNFaceDetectorSSD::DNNFaceDetectorSSD()
    : DNNFaceDetectorBase(1.0,
                          cv::Scalar(104.0, 177.0, 123.0),
                          cv::Size(300, 300))
{
    loadModels();
}

bool DNNFaceDetectorSSD::loadModels()
{
    QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine"),
                                             QStandardPaths::LocateDirectory);

    QString model   = QLatin1String("deploy.prototxt");
    QString data    = QLatin1String("res10_300x300_ssd_iter_140000_fp16.caffemodel");

    QString nnmodel = appPath + QLatin1Char('/') + model;
    QString nndata  = appPath + QLatin1Char('/') + data;

    if (QFileInfo::exists(nnmodel) && QFileInfo::exists(nndata))
    {
        try
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "SSD model:" << model << ", SSD data:" << data;

#ifdef Q_OS_WIN

            net = cv::dnn::readNetFromCaffe(nnmodel.toLocal8Bit().constData(),
                                            nndata.toLocal8Bit().constData());

#else

            net = cv::dnn::readNetFromCaffe(nnmodel.toStdString(),
                                            nndata.toStdString());

#endif

#if (OPENCV_VERSION == QT_VERSION_CHECK(4, 7, 0))

            net.enableWinograd(false);

#endif

        }
        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "cv::Exception:" << e.what();

            return false;
        }
        catch (...)
        {
           qCWarning(DIGIKAM_FACEDB_LOG) << "Default exception from OpenCV";

           return false;
        }
    }
    else
    {
        qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot found faces engine DNN model" << model << "or" << data;
        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces detection feature cannot be used!";

        return false;
    }

    return true;
}

void DNNFaceDetectorSSD::detectFaces(const cv::Mat& inputImage,
                                     const cv::Size& paddedSize,
                                     std::vector<cv::Rect>& detectedBboxes)
{
    if (inputImage.empty())
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Invalid image given, not detecting faces.";
        return;
    }

    cv::Mat detection;
    cv::Mat inputBlob = cv::dnn::blobFromImage(inputImage, scaleFactor, inputImageSize, meanValToSubtract, true, false);

    if (!net.empty())
    {
        QMutexLocker lock(&mutex);
        net.setInput(inputBlob);
        detection = net.forward();
    }

    postprocess(detection, paddedSize, detectedBboxes);
}

void DNNFaceDetectorSSD::postprocess(cv::Mat detection,
                                     const cv::Size& paddedSize,
                                     std::vector<cv::Rect>& detectedBboxes) const
{
    std::vector<float> goodConfidences, doubtConfidences, confidences;
    std::vector<cv::Rect> goodBoxes, doubtBoxes, boxes;

    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    // TODO: model problem, confidence of ssd output too low ===> false detection

    for (int i = 0 ; i < detectionMat.rows ; ++i)
    {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > confidenceThreshold)
        {
            float leftRatio   = detectionMat.at<float>(i, 3);
            float topRatio    = detectionMat.at<float>(i, 4);
            float rightRatio  = detectionMat.at<float>(i, 5);
            float bottomRatio = detectionMat.at<float>(i, 6);

            int left          = (int)(leftRatio   * inputImageSize.width);
            int right         = (int)(rightRatio  * inputImageSize.width);
            int top           = (int)(topRatio    * inputImageSize.height);
            int bottom        = (int)(bottomRatio * inputImageSize.height);

            selectBbox(paddedSize,
                       confidence,
                       left,
                       right,
                       top,
                       bottom,
                       goodConfidences,
                       goodBoxes,
                       doubtConfidences,
                       doubtBoxes);
        }
    }
/*
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "nb of doubtbox = " << doubtBoxes.size();
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "nb of goodbox = " << goodBoxes.size();
*/
    if (goodBoxes.empty())
    {
        boxes       = doubtBoxes;
        confidences = doubtConfidences;
    }
    else
    {
        boxes       = goodBoxes;
        confidences = goodConfidences;
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with lower confidences

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

    // Get detected bounding boxes

    for (size_t i = 0 ; i < indices.size() ; ++i)
    {
        cv::Rect bbox = boxes[indices[i]];
        correctBbox(bbox, paddedSize);
        detectedBboxes.push_back(cv::Rect(bbox.x, bbox.y, bbox.width, bbox.height));
    }
}

} // namespace Digikam
