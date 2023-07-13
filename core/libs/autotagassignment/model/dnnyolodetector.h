/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform YOLO neural network inference
 *               for face detection (including yolo versions to benchmark).
 * 
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DNN_DETECTOR_YOLO_H
#define DIGIKAM_DNN_DETECTOR_YOLO_H

#include "dnnbasedetectormodel.h"

namespace Digikam
{

enum class YoloVersions
{
    YOLOV5NANO = 0,
    YOLOV5XLARGE
};

class DIGIKAM_EXPORT DNNYoloDetector: public DNNBaseDetectorModel
{

public:

    explicit DNNYoloDetector(YoloVersions modelVersion = YoloVersions::YOLOV5NANO);
    ~DNNYoloDetector()                                               override;

public:

    QVector<QString> getPredefinedClasses()                        const;

public: 

    bool loadModels();

    void detectObjects(const cv::Mat& inputImage,
                       QMap<QString, QVector<QRect>>& detectedBoxes) override;

    QVector<QString>  loadCOCOClass();    ///< load 80 predifined classes for object detection "coco.names"

private:

    std::vector<cv::String> getOutputsNames()                      const;

    std::vector<cv::Mat> preprocess(const cv::Mat& inputImage);

    void postprocess(const cv::Mat& inputImage,
                     const std::vector<cv::Mat>& outs,
                     QMap<QString, QVector<QRect>>& detectedBoxes) const;

private:

    // Disable
    DNNYoloDetector(const DNNYoloDetector&)            = delete;
    DNNYoloDetector& operator=(const DNNYoloDetector&) = delete;

private:

    QVector<QString> predefinedClasses;     
    YoloVersions yoloVersion;
};

} // namespace Digikam

#endif // DIGIKAM_DNN_DETECTOR_YOLO_H
