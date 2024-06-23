/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Base class to perform low-level neural network inference
 *               for object detection and object classification
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

namespace Digikam
{

class DIGIKAM_GUI_EXPORT DNNBaseDetectorModel
{

public:

    explicit DNNBaseDetectorModel();
    explicit DNNBaseDetectorModel(float scale, const cv::Scalar& val, const cv::Size& inputImgSize);
    virtual ~DNNBaseDetectorModel();

    /**
     * detectObjects return the predicted objects and localization as well (if we use deeplearning for object detection like YOLO, etc)
     * otherwise the map whose the key is the objects name and their values are empty
     */
    virtual QHash<QString, QVector<QRect> > detectObjects(const cv::Mat& inputImage) = 0;

    /**
     * detectObjects in batch images (fixed batch size)
     */
    virtual QList<QHash<QString, QVector<QRect> > > detectObjects(const std::vector<cv::Mat>& inputBatchImages) = 0;

    /**
     * get predefined objects according to selected model
     */
    virtual QList<QString> getPredefinedClasses() const = 0;

    /**
     * generateObjects in one image return just the predicted objects without locations of objects
     * using for the assignment tagging names
     */
    QList<QString> generateObjects(const cv::Mat& inputImage);

    /**
     * generateObjects in batch images return just the predicted objects without locations of objects
     * using for the assignment tagging names
     */
    QList<QList<QString> > generateObjects(const std::vector<cv::Mat>& inputImage);

    /**
     * return the input Image Size from Deep NN model
     */
   cv::Size getinputImageSize() const;

public:

    double showInferenceTime();

public:

    static float confidenceThreshold;    ///< Threshold for bbox detection. It can be init and changed in the GUI
    static float nmsThreshold;           ///< Threshold for nms suppression
    static float scoreThreshold;         ///< Threshold for class detection score

protected:

    float        scaleFactor = 1.0F;
    cv::Scalar   meanValToSubtract;
    cv::Size     inputImageSize;

    cv::dnn::Net net;
    QMutex       mutex;

private:

    // Disable
    DNNBaseDetectorModel(const DNNBaseDetectorModel&)            = delete;
    DNNBaseDetectorModel& operator=(const DNNBaseDetectorModel&) = delete;
};

} // namespace Digikam
