/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Derived class to perform Resnet neural network inference
 *               for object detection (including yolo versions to benchmark).
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DNN_DETECTOR_RESNET_H
#define DIGIKAM_DNN_DETECTOR_RESNET_H

#include "dnnbasedetectormodel.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNResnetDetector: public DNNBaseDetectorModel
{

public:

    explicit DNNResnetDetector();
    ~DNNResnetDetector()                                                      override;

public:

    QList<QString>    getPredefinedClasses()                                           const         override;
    bool              loadModels();
    QList<QString>    loadImageNetClass();    ///< load 80 predifined classes for object detection "..."

public:

    QHash<QString, QVector<QRect>>        detectObjects(const cv::Mat& inputImage)                    override;
    QList<QHash<QString, QVector<QRect>>> detectObjects(const std::vector<cv::Mat>& inputBatchImages) override;

private:

    std::vector<cv::String> getOutputsNames()                                          const;

    std::vector<cv::Mat> preprocess(const cv::Mat& inputImage);
    std::vector<cv::Mat> preprocess(const std::vector<cv::Mat>& inputBatchImages);

    QHash<QString, QVector<QRect>>        postprocess(const cv::Mat& inputImage,
                                                     const cv::Mat& out)               const;

    QList<QHash<QString, QVector<QRect>>> postprocess(const std::vector<cv::Mat>& inputBatchImages,
                                                     const std::vector<cv::Mat>& outs) const;

private:

    // Disable
    DNNResnetDetector(const DNNResnetDetector&)            = delete;
    DNNResnetDetector& operator=(const DNNResnetDetector&) = delete;

private:

    QList<QString>   predefinedClasses;
};

} // namespace Digikam

#endif // DIGIKAM_DNN_DETECTOR_RESNET_H
