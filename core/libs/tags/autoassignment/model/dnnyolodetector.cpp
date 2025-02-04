/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : YOLO DNN inference for object detection.
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnyolodetector.h"

// Qt includes

#include <QElapsedTimer>
#include <QMutexLocker>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "dnnmodelmanager.h"
#include "dnnmodelnet.h"

namespace Digikam
{

DNNYoloDetector::DNNYoloDetector(YoloVersions modelVersion)
    : DNNBaseDetectorModel(1.0F / 255.0F, cv::Scalar(0.0 ,0.0 ,0.0), cv::Size(320, 320)),
      yoloVersion         (modelVersion)
{
    if (this->loadModels())
    {
        predefinedClasses = loadDetectionClasses();
    }
}

bool DNNYoloDetector::loadModels()
{
    switch (yoloVersion)
    {
        case (YoloVersions::YOLOV5NANO):
        {
            model = DNNModelManager::instance()->getModel(QLatin1String("YOLOv5n"), DNNModelUsage::DNNUsageObjectDetection);
            break;
        }

        case (YoloVersions::YOLOV5XLARGE):
        {
            model = DNNModelManager::instance()->getModel(QLatin1String("YOLOv5xl"), DNNModelUsage::DNNUsageObjectDetection);
            break;
        }

        case (YoloVersions::RESNET50):
        {
            // NOTE: nothing to do here.

            break;
        }
    }

    if (model && !model->modelLoaded)   // cppcheck-suppress duplicateCondition
    {
        try
        {
            cv::dnn::Net net = static_cast<DNNModelNet*>(model)->getNet();
        }
        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();

            return false;
        }
        catch (...)
        {
           qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";

           return false;
        }
    }

    if (model && !model->modelLoaded)   // cppcheck-suppress duplicateCondition
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Cannot find object detection DNN model";

        return false;
    }

    return true;
}

QHash<QString, QVector<QRect> > DNNYoloDetector::postprocess(const cv::Mat& inputImage,
                                                             const cv::Mat& out) const
{
    QHash<QString, QVector<QRect> > detectedBoxes;

    const float confidenceThreshold = model->getThreshold(uiConfidenceThreshold);

    if (predefinedClasses.isEmpty())
    {
        return detectedBoxes;
    }

    std::vector<int>      class_ids;
    std::vector<float>    confidences;
    std::vector<cv::Rect> boxes;

    try
    {
        float x_factor = float(inputImage.cols) / float(inputImageSize.width);
        float y_factor = float(inputImage.rows) / float(inputImageSize.height);
        float* data    = reinterpret_cast<float*>(out.data);

        // Calculate the size of the data array and number of outputs.
        // NOTE: output is a cv::Mat vector of [1 x (250200 * 85)].

        size_t data_size = out.total() * out.channels();
        int rows         = data_size / 85;

        for (int i = 0 ; i < rows ; ++i)
        {
            float confidence = data[4];

            // Discard bad detections and continue.

            if (confidence >= confidenceThreshold)
            {
                float* const classes_scores = data + 5;

                // Create a 1x85 Mat and store class scores of 80 classes.

                cv::Mat scores(1, predefinedClasses.size(), CV_32FC1, classes_scores);

                // Perform minMaxLoc and acquire the index of best class score.

                cv::Point class_id;
                double max_class_score = 0.0;
                cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                // Continue if the class score is above the threshold.

                if (max_class_score > scoreThreshold)
                {
                    // Store class ID and confidence in the pre-defined respective vectors.

                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);

                    // Center.

                    float centerX = data[0];
                    float centerY = data[1];

                    // Box dimension.

                    float w       = data[2];
                    float h       = data[3];

                    // Bounding box coordinates.

                    int left      = int((centerX - 0.5 * w) * x_factor);
                    int top       = int((centerY - 0.5 * h) * y_factor);
                    int width     = int(w                   * x_factor);
                    int height    = int(h                   * y_factor);

                    // Store good detections in the boxes vector.

                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }

            // Jump to the next row.

            data += 85;
        }

        // Perform non maximum suppression to eliminate redundant overlapping boxes with lower confidences.

        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

        for (const auto& id : indices)
        {
            cv::Rect bbox = boxes[id];
            QString label = predefinedClasses[class_ids[id]];
            detectedBoxes[label].push_back(QRect(bbox.x, bbox.y, bbox.width, bbox.height));
        }
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return detectedBoxes;
}

} // namespace Digikam
