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

#include "dnnbasedetectormodel.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

float DNNBaseDetectorModel::confidenceThreshold = 0.5F;
float DNNBaseDetectorModel::nmsThreshold        = 0.4F;
float DNNBaseDetectorModel::scoreThreshold      = 0.45F;

// --------------------------------------------------------------------------------------------------------

DNNBaseDetectorModel::DNNBaseDetectorModel()
  : scaleFactor(1.0)
{
}

DNNBaseDetectorModel::DNNBaseDetectorModel(float scale,
                                           const cv::Scalar& val,
                                           const cv::Size& inputImageSize)
    : scaleFactor       (scale),
      meanValToSubtract (val),
      inputImageSize    (inputImageSize)
{
}

DNNBaseDetectorModel::~DNNBaseDetectorModel()
{
}

QList<QString> DNNBaseDetectorModel::generateObjects(const cv::Mat& inputImage)
{
    QHash<QString, QVector<QRect> > results = detectObjects(inputImage);
    QList<QString> objectNames;

    for (QHash<QString, QVector<QRect> >::const_iterator it = results.constBegin() ;
         it != results.constEnd() ; ++it)
    {
        objectNames.append(it.key());
    }

    return objectNames;
}

QList<QList<QString> > DNNBaseDetectorModel::generateObjects(const std::vector<cv::Mat>& inputBatchImages)
{
    QList<QHash<QString, QVector<QRect> > > results = detectObjects(inputBatchImages);
    QList<QList<QString> > objectNamesList;

    for (auto detectedBoxes : std::as_const(results))
    {
        QList<QString> objectNames;

        for (QHash<QString, QVector<QRect> >::const_iterator it = detectedBoxes.constBegin() ;
             it != detectedBoxes.constEnd() ; ++it)
        {
            objectNames.append(it.key());
        }

        objectNamesList.append(objectNames);
    }

    return objectNamesList;
}

cv::Size DNNBaseDetectorModel::getinputImageSize() const
{
    return inputImageSize;
}

double DNNBaseDetectorModel::showInferenceTime()
{
    // Put efficiency information.
    // The function getPerfProfile returns the overall time for
    // inference(t) and the timings for each of the layers(in layersTimes).

    std::vector<double> layersTimes;
    double freq = cv::getTickFrequency() / 1000.0;

    return (net.getPerfProfile(layersTimes) / freq);
}

} // namespace Digikam
