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

#include "dnnfacedetectoryolo.h"

// Qt includes

#include <QList>
#include <QRect>
#include <QString>
#include <QElapsedTimer>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"

namespace Digikam
{

DNNFaceDetectorYOLO::DNNFaceDetectorYOLO()
    : DNNFaceDetectorBase(1.0F / 255.0F, cv::Scalar(0.0, 0.0, 0.0), cv::Size(416, 416))
{
    loadModels();
}

DNNFaceDetectorYOLO::~DNNFaceDetectorYOLO()
{
}

bool DNNFaceDetectorYOLO::loadModels()
{
    QString model   = QLatin1String("yolov3-face.cfg");
    QString data    = QLatin1String("yolov3-wider_16000.weights");

    QString nnmodel = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                             QString::fromLatin1("facesengine/%1").arg(model));
    QString nndata  = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                             QString::fromLatin1("facesengine/%1").arg(data));

    if (!nnmodel.isEmpty() && !nndata.isEmpty())
    {
        try
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "YOLO model:" << nnmodel << ", YOLO data:" << nndata;

#ifdef Q_OS_WIN

            net = cv::dnn::readNetFromDarknet(nnmodel.toLocal8Bit().constData(),
                                              nndata.toLocal8Bit().constData());

#else

            net = cv::dnn::readNetFromDarknet(nnmodel.toStdString(),
                                              nndata.toStdString());

#endif

            net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
            net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
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

    mutex.lock();
    {
        net.setInput(inputBlob);
        timer.start();
        net.forward(outs, getOutputsNames());
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "forward YOLO detection in" << timer.elapsed() << "ms";
    }
    mutex.unlock();

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

    for (size_t i = 0 ; i < outs.size() ; ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.

        float* data = reinterpret_cast<float*>(outs[i].data);

        for (int j = 0 ; j < outs[i].rows ; ++j, data += outs[i].cols)
        {
            cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);

            // Get the value and location of the maximum score

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

/** Get the names of the output layers
 */
std::vector<cv::String> DNNFaceDetectorYOLO::getOutputsNames() const
{
    static std::vector<cv::String> names;

    if (names.empty())
    {
        // Get the indices of the output layers, i.e. the layers with unconnected outputs

        std::vector<int> outLayers = net.getUnconnectedOutLayers();

        // Get the names of all the layers in the network

        std::vector<cv::String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names

        names.resize(outLayers.size());

        for (size_t i = 0 ; i < outLayers.size() ; ++i)
        {
            names[i] = layersNames[outLayers[i] - 1];
        }
    }

    return names;
}

} // namespace Digikam
