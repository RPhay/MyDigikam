/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform SSD neural network inference
 *               for face detection
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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
#include "dnnmodelnet.h"
#include "dnnmodelmanager.h"

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

    model = DNNModelManager::instance()->getModel(QLatin1String("MobilenetSSD"), DNNModelUsage::DNNUsageFaceDetection);

    if (model && !model->modelLoaded)
    {
        try
        {
            // NOTE This will throw an exception if the model can't be loaded.

            cv::dnn::Net net = static_cast<DNNModelNet*>(model)->getNet();

            qCDebug(DIGIKAM_FACEDB_LOG) << "SSD model:" << model->info.displayName
                                        << ", SSD data:" << model->info.configName;
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

    if (model && model->modelLoaded)
    {
        qCDebug(DIGIKAM_FACEDB_LOG) << "SSD model:" << model->info.displayName << "ready";
    }
    else
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "Face detection model: SSD not loaded";
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

    if (model && !static_cast<DNNModelNet*>(model)->getNet().empty())
    {
        QMutexLocker lock(&(model->mutex));
        static_cast<DNNModelNet*>(model)->getNet().setInput(inputBlob);
        detection = static_cast<DNNModelNet*>(model)->getNet().forward();
    }
    else
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "Face detection model: SSD not loaded. Processed 0 images.";
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

    const float confidenceThreshold = model->getThreshold(uiConfidenceThreshold);

    // TODO: model problem, confidence of ssd output too low ==> false detection.

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

    // Perform non maximum suppression to eliminate redundant overlapping boxes with lower confidences.

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

    // Get detected bounding boxes.

    for (size_t i = 0 ; i < indices.size() ; ++i)
    {
        cv::Rect bbox = boxes[indices[i]];
        correctBbox(bbox, paddedSize);
        detectedBboxes.push_back(cv::Rect(bbox.x, bbox.y, bbox.width, bbox.height));
    }
}

} // namespace Digikam
