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
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnfacedetectoryolo.h"

// Qt includes

#include <QList>
#include <QRect>
#include <QString>
#include <QFileInfo>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "dnnmodelnet.h"
#include "dnnmodelmanager.h"

namespace Digikam
{

DNNFaceDetectorYOLO::DNNFaceDetectorYOLO()
    : DNNFaceDetectorBase(1.0F / 255.0F,
                          cv::Scalar(0.0, 0.0, 0.0),
                          cv::Size(416, 416))
{
    loadModels();
}

bool DNNFaceDetectorYOLO::loadModels()
{
    model = DNNModelManager::instance()->getModel(QLatin1String("YOLOv3"), DNNModelUsage::DNNUsageFaceDetection);

    if (model && !model->modelLoaded)
    {
        try
        {
            // NOTE: this will throw an exception if the model can't be loaded.

            cv::dnn::Net net = static_cast<DNNModelNet*>(model)->getNet();

            qCDebug(DIGIKAM_FACEDB_LOG) << "YOLOv3 model:" << model->info.displayName
                                        << ", YOLOv3 data:" << model->info.configName;
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
        qCWarning(DIGIKAM_FACEDB_LOG) << "Face detection model: YOLOv3 not loaded";
        return false;
    }

    return true;
}

void DNNFaceDetectorYOLO::detectFaces(const cv::Mat& inputImage,
                                      const cv::Size& paddedSize,
                                      std::vector<cv::Rect>& detectedBboxes)
{
    QElapsedTimer timer;

    if (inputImage.empty())
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Invalid image given, not detecting faces.";

        return;
    }

    cv::Mat inputBlob = cv::dnn::blobFromImage(inputImage, scaleFactor, inputImageSize, meanValToSubtract, true, false);
    std::vector<cv::Mat> outs;

    if (model && !static_cast<DNNModelNet*>(model)->getNet().empty())
    {
        QMutexLocker lock(&(model->mutex));

        static_cast<DNNModelNet*>(model)->getNet().setInput(inputBlob);
        timer.start();
        static_cast<DNNModelNet*>(model)->getNet().forward(outs, getOutputsNames());

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "forward YOLO detection in" << timer.elapsed() << "ms";
    }
    else
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "Face detection model: YOLOv3 not loaded. Processed 0 images.";
    }


    timer.start();

    postprocess(outs, paddedSize, detectedBboxes);

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "postprocess YOLO detection in" << timer.elapsed() << "ms";
}

void DNNFaceDetectorYOLO::postprocess(const std::vector<cv::Mat>& outs,
                                      const cv::Size& paddedSize,
                                      std::vector<cv::Rect>& detectedBboxes) const
{
    std::vector<float>    goodConfidences;
    std::vector<float>    doubtConfidences;
    std::vector<float>    confidences;
    std::vector<cv::Rect> goodBoxes;
    std::vector<cv::Rect> doubtBoxes;
    std::vector<cv::Rect> boxes;

    const float confidenceThreshold = model->getThreshold(uiConfidenceThreshold);

    for (size_t i = 0 ; i < outs.size() ; ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.

        float* data = reinterpret_cast<float*>(outs[i].data);

        for (int j = 0 ; j < outs[i].rows ; ++j, data += outs[i].cols)
        {
            cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);

            // Get the value and location of the maximum score.

            double confidence;
            cv::minMaxLoc(scores, nullptr, &confidence, nullptr, nullptr);

            if (confidence > confidenceThreshold)
            {
                int centerX = (int)(data[0] * inputImageSize.width);
                int centerY = (int)(data[1] * inputImageSize.height);
                int width   = (int)(data[2] * inputImageSize.width);
                int height  = (int)(data[3] * inputImageSize.height);

                int left    = centerX - width  / 2;
                int right   = centerX + width  / 2;
                int top     = centerY - height / 2;
                int bottom  = centerY + height / 2;

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
    }

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "nb of doubtbox = " << doubtBoxes.size();
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "nb of goodbox = "  << goodBoxes.size();

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

/**
 * Get the names of the output layers.
 */
std::vector<cv::String> DNNFaceDetectorYOLO::getOutputsNames() const
{
    static std::vector<cv::String> names;

    if (!static_cast<DNNModelNet*>(model)->getNet().empty() && names.empty())
    {
        // Get the indices of the output layers, i.e. the layers with unconnected outputs.

        std::vector<int> outLayers          = static_cast<DNNModelNet*>(model)->getNet().getUnconnectedOutLayers();

        // Get the names of all the layers in the network.

        std::vector<cv::String> layersNames = static_cast<DNNModelNet*>(model)->getNet().getLayerNames();

        // Get the names of the output layers in names.

        names.resize(outLayers.size());

        for (size_t i = 0 ; i < outLayers.size() ; ++i)
        {
            names[i] = layersNames[outLayers[i] - 1];
        }
    }

    return names;
}

} // namespace Digikam
