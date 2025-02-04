/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Low-level DNN base class for object detection and classification.
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// C++ includes

#include <vector>

// Qt includes

#include <QHash>
#include <QVector>
#include <QString>
#include <QRect>
#include <QMutex>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"
#include "dnnmodelbase.h"

namespace Digikam
{

/**
 * @brief Base class to perform low-level neural network inference
 * for object detection and object classification
 */
class DIGIKAM_GUI_EXPORT DNNBaseDetectorModel
{

public:

    explicit DNNBaseDetectorModel();
    explicit DNNBaseDetectorModel(float scale, const cv::Scalar& val, const cv::Size& inputImgSize);
    virtual ~DNNBaseDetectorModel() = default;

    QList<QString> loadDetectionClasses();

    std::vector<cv::Mat> preprocess(const cv::Mat& inputImage);
    std::vector<cv::Mat> preprocess(const std::vector<cv::Mat>& inputBatchImages);

    QList<QHash<QString, QVector<QRect> > > postprocess(const std::vector<cv::Mat>& inputBatchImages,
                                                        const std::vector<cv::Mat>& outs)   const;

    virtual QHash<QString, QVector<QRect> > postprocess(const cv::Mat& inputImage,
                                                        const cv::Mat& out)                 const   = 0;

    std::vector<cv::String> getOutputsNames()                                               const;

    /**
     * @return The predicted objects and localization as well (if we use deeplearning for object detection like YOLO, etc)
     * otherwise the map whose the key is the objects name and their values are empty.
     */
    virtual QHash<QString, QVector<QRect> > detectObjects(const cv::Mat& inputImage);

    /**
     * @return Detected Objects in batch images (fixed batch size).
     */
    virtual QList<QHash<QString, QVector<QRect> > > detectObjects(const std::vector<cv::Mat>& inputBatchImages);

    /**
     * @return Predefined objects according to selected model.
     */
    virtual QList<QString> getPredefinedClasses()                                           const;

    /**
     * @return From one image just the predicted objects without locations of objects
     * using for the assignment tagging names.
     */
    QList<QString> generateObjects(const cv::Mat& inputImage);

    /**
     * @return In batch images just the predicted objects without locations of objects
     * using for the assignment tagging names.
     */
    QList<QList<QString> > generateObjects(const std::vector<cv::Mat>& inputImage);

    /**
     * @return The input Image Size from Deep Neural Network model.
     */
    cv::Size getinputImageSize()                                                            const;

public:

    double showInferenceTime();

public:

    static int   uiConfidenceThreshold;  ///< @brief Threshold for bbox detection. It can be init and changed in the GUI.
    static float nmsThreshold;           ///< @brief Threshold for nms suppression.
    static float scoreThreshold;         ///< @brief Threshold for class detection score.

protected:

    // TODO: Set these from the DNNModel.

    float           scaleFactor = 1.0F;
    cv::Scalar      meanValToSubtract;
    cv::Size        inputImageSize;
    QList<QString>  predefinedClasses;

    DNNModelBase*   model       = nullptr;

protected:

    virtual bool loadModels()   = 0;

private:

    // Disable
    DNNBaseDetectorModel(const DNNBaseDetectorModel&)            = delete;
    DNNBaseDetectorModel& operator=(const DNNBaseDetectorModel&) = delete;
};

} // namespace Digikam
