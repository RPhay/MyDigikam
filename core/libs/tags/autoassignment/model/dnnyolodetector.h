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

#pragma once

#include "dnnbasedetectormodel.h"

namespace Digikam
{

enum class YoloVersions
{
    YOLOV5NANO = 0,         ///< yolov5n_batch_16_s320.onnx
    YOLOV5XLARGE,           ///< yolov5x_batch_16_s320.onnx
    RESNET50                ///< resnet50.onnx
};

/**
 * @brief Derived class to perform YOLO neural network inference
 * for object detection (including yolo versions to benchmark).
 */
class DIGIKAM_GUI_EXPORT DNNYoloDetector: public DNNBaseDetectorModel
{

public:

    explicit DNNYoloDetector(YoloVersions modelVersion = YoloVersions::YOLOV5NANO);
    ~DNNYoloDetector()                                                                      override = default;

protected:

    bool loadModels()                                                                       override;

private:

    QHash<QString, QVector<QRect> > postprocess(const cv::Mat& inputImage,
                                                const cv::Mat& out)                   const override;

private:

    // Disable
    DNNYoloDetector(const DNNYoloDetector&)            = delete;
    DNNYoloDetector& operator=(const DNNYoloDetector&) = delete;

private:

    YoloVersions   yoloVersion;
};

} // namespace Digikam
