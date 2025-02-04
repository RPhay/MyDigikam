/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Resnet DNN inference for object detection.
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnresnetdetector.h"

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

DNNResnetDetector::DNNResnetDetector()
    : DNNBaseDetectorModel(1.0F / 255.0F,
                           cv::Scalar(0.0 ,0.0 ,0.0),
                           cv::Size(224, 224))
{
    if (this->loadModels())
    {
        predefinedClasses = loadDetectionClasses();
    }
}

bool DNNResnetDetector::loadModels()
{
    model = DNNModelManager::instance()->getModel(QLatin1String("ResNet50"), DNNModelUsage::DNNUsageObjectDetection);

    if (model && !model->modelLoaded)   // cppcheck-suppress duplicateCondition
    {
        try
        {
            cv::dnn::Net net = static_cast<DNNModelNet*>(model)->getNet();  // check we can load the DNN net
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
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Cannot find object classification DNN model";

        return false;
    }

    return true;
}

QHash<QString, QVector<QRect> > DNNResnetDetector::postprocess(const cv::Mat& /*inputImage*/,
                                                               const cv::Mat& out) const
{
    QHash<QString, QVector<QRect> > detectedBoxes;

    if (predefinedClasses.isEmpty())
    {
        return detectedBoxes;
    }

    cv::Point classIdPoint;
    double final_prob = 0.0;

    minMaxLoc(out.reshape(1, 1), 0, &final_prob, 0, &classIdPoint);
    int label_id      = classIdPoint.x;

    QString label     = predefinedClasses[label_id];
    detectedBoxes[label].push_back( { } );

    return detectedBoxes;
}

} // namespace Digikam
