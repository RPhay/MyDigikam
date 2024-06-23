/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Derived class to perform YOLO neural network inference
 *               for object detection (including yolo versions to benchmark).
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
    YOLOV5NANO = 0,
    YOLOV5XLARGE,
    RESNET50
};

class DIGIKAM_GUI_EXPORT DNNYoloDetector: public DNNBaseDetectorModel
{

public:

    explicit DNNYoloDetector(YoloVersions modelVersion = YoloVersions::YOLOV5NANO);
    ~DNNYoloDetector()                                                                                  override;

public:

    QList<QString>    getPredefinedClasses()                                              const         override;
    bool              loadModels();

    /**
     * load 80 predefined classes for object detection "coco.names".
     */
    QList<QString>    loadCOCOClass();

public:

    QHash<QString, QVector<QRect> >         detectObjects(const cv::Mat& inputImage)                    override;
    QList<QHash<QString, QVector<QRect> > > detectObjects(const std::vector<cv::Mat>& inputBatchImages) override;

private:

    std::vector<cv::String> getOutputsNames()                                             const;

    std::vector<cv::Mat> preprocess(const cv::Mat& inputImage);
    std::vector<cv::Mat> preprocess(const std::vector<cv::Mat>& inputBatchImages);

    QHash<QString, QVector<QRect> >        postprocess(const cv::Mat& inputImage,
                                                       const cv::Mat& out)                const;

    QList<QHash<QString, QVector<QRect> > > postprocess(const std::vector<cv::Mat>& inputBatchImages,
                                                        const std::vector<cv::Mat>& outs) const;

private:

    // Disable
    DNNYoloDetector(const DNNYoloDetector&)            = delete;
    DNNYoloDetector& operator=(const DNNYoloDetector&) = delete;

private:

    QList<QString> predefinedClasses;
    YoloVersions   yoloVersion;
};

} // namespace Digikam
