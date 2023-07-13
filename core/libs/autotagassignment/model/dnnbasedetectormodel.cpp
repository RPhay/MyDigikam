/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Base class to perform low-level neural network inference
 *               for object detection
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnbasedetectormodel.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

float DNNBaseDetectorModel::confidenceThreshold  = 0.5F;
float DNNBaseDetectorModel::nmsThreshold         = 0.4F;
float DNNBaseDetectorModel::scoreThreshold       = 0.45F;

// --------------------------------------------------------------------------------------------------------

DNNBaseDetectorModel::DNNBaseDetectorModel()
  : scaleFactor(1.0)
{
}

DNNBaseDetectorModel::DNNBaseDetectorModel(float scale,
                                          const cv::Scalar& val,
                                          const cv::Size& inputImageSize)
    : scaleFactor (scale),
      meanValToSubtract(val),
      inputImageSize(inputImageSize)
{
}

DNNBaseDetectorModel::~DNNBaseDetectorModel()
{
}

double DNNBaseDetectorModel::showInferenceTime()
{
    // Put efficiency information.
    // The function getPerfProfile returns the overall time for     
    // inference(t) and the timings for each of the layers(in layersTimes).
    std::vector<double> layersTimes;
    double freq = cv::getTickFrequency() / 1000;
    return net.getPerfProfile(layersTimes) / freq;
}

} // namespace Digikam
